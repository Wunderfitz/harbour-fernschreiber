/*
    Copyright (C) 2026 Fernschreiber contributors

    This file is part of Fernschreiber and is licensed under the GNU General
    Public License v3.0 (see LICENSE). Adapted from RooTelegram (GPL-3.0).
*/
#include "callaudiorouter.h"

#include <QTimer>
#include <QStringList>
#include <cstring>
#include <pulse/pulseaudio.h>

namespace {

struct SinkScan {
    pa_threaded_mainloop *ml = nullptr;
    QString sink;
    QString speaker;
    QString earpiece;
};

void ctxStateCb(pa_context * /*c*/, void *userdata)
{
    pa_threaded_mainloop_signal(static_cast<pa_threaded_mainloop *>(userdata), 0);
}

// First sink that has BOTH a "speaker" and an "earpiece/handset/receiver" port
// (so it skips sink.null). Keyword match on port name/description -> device-agnostic.
void sinkInfoCb(pa_context * /*c*/, const pa_sink_info *info, int eol, void *userdata)
{
    SinkScan *scan = static_cast<SinkScan *>(userdata);
    if (eol) {
        pa_threaded_mainloop_signal(scan->ml, 0);
        return;
    }
    if (!info || !scan->sink.isEmpty()) {
        return;
    }
    QString speaker, earpiece;
    for (uint32_t p = 0; p < info->n_ports; ++p) {
        const pa_sink_port_info *port = info->ports[p];
        if (!port || !port->name) {
            continue;
        }
        const QString name = QString::fromUtf8(port->name).toLower();
        const QString desc = QString::fromUtf8(port->description ? port->description : "").toLower();
        if (speaker.isEmpty() && (name.contains("speaker") || desc.contains("speaker"))) {
            speaker = QString::fromUtf8(port->name);
        }
        if (earpiece.isEmpty()
                && (name.contains("earpiece") || name.contains("handset") || name.contains("receiver")
                    || desc.contains("earpiece") || desc.contains("handset") || desc.contains("receiver"))) {
            earpiece = QString::fromUtf8(port->name);
        }
    }
    if (!speaker.isEmpty() && !earpiece.isEmpty()) {
        scan->sink = QString::fromUtf8(info->name);
        scan->speaker = speaker;
        scan->earpiece = earpiece;
    }
}

struct SinkInputScan {
    pa_threaded_mainloop *ml = nullptr;
    uint32_t index = PA_INVALID_INDEX;
    bool found = false;
    uint8_t channels = 2;
    int mute = 0;
};

// Probe fast until the stream shows up, then keep watching at a calmer pace.
const int PROBE_INTERVAL_MS = 500;
const int WATCHDOG_INTERVAL_MS = 2000;

void sinkInputInfoCb(pa_context * /*c*/, const pa_sink_input_info *info, int eol, void *userdata)
{
    SinkInputScan *scan = static_cast<SinkInputScan *>(userdata);
    if (eol) {
        pa_threaded_mainloop_signal(scan->ml, 0);
        return;
    }
    if (!info || !info->proplist) {
        return;
    }
    const char *app = pa_proplist_gets(info->proplist, "application.name");
    if (app && (std::strstr(app, "WEBRTC") || std::strstr(app, "VoiceEngine"))) {
        scan->index = info->index;
        scan->channels = info->volume.channels > 0 ? info->volume.channels : 2;
        scan->mute = info->mute;
        scan->found = true;
    }
}

struct SinkVolumeScan {
    pa_threaded_mainloop *ml = nullptr;
    pa_cvolume volume;
    bool found = false;
};

void sinkVolumeInfoCb(pa_context * /*c*/, const pa_sink_info *info, int eol, void *userdata)
{
    SinkVolumeScan *scan = static_cast<SinkVolumeScan *>(userdata);
    if (eol) {
        pa_threaded_mainloop_signal(scan->ml, 0);
        return;
    }
    if (!info) {
        return;
    }
    scan->volume = info->volume;
    scan->found = true;
}

} // namespace

CallAudioRouter::CallAudioRouter(QObject *parent)
    : QObject(parent)
    , m_retryTimer(new QTimer(this))
    , m_mainloop(nullptr)
    , m_context(nullptr)
    , m_speakerOn(true)   // hands-free by default (audible for video calls on a desk)
    , m_sinkVolumeSaved(false)
    , m_routedIndex(PA_INVALID_INDEX)
{
    // The WebRTC stream appears shortly after the call connects — retry until found.
    // Once it is routed the timer keeps running as a watchdog rather than stopping:
    // tgcalls tears the playout stream down and builds a new one when the media
    // configuration changes (switching camera does this), and the replacement is
    // born muted on the media sink like the first one. Without the watchdog the
    // remote party goes silent mid-call while the microphone keeps working.
    m_retryTimer->setInterval(PROBE_INTERVAL_MS);
    connect(m_retryTimer, &QTimer::timeout, this, [this]() {
        const quint32 previous = m_routedIndex;
        if (routeWebrtcToCallSink() && m_routedIndex != previous) {
            // Set the call sink's port (speaker/earpiece). This also WAKES the
            // primary_output sink out of SUSPENDED, so the moved stream plays.
            setSpeakerphone(m_speakerOn);
            m_retryTimer->setInterval(WATCHDOG_INTERVAL_MS);
        }
    });
}

CallAudioRouter::~CallAudioRouter()
{
    stop();
    if (m_context) {
        pa_context_unref(static_cast<pa_context *>(m_context));
        m_context = nullptr;
    }
    if (m_mainloop) {
        pa_threaded_mainloop_stop(static_cast<pa_threaded_mainloop *>(m_mainloop));
        pa_threaded_mainloop_free(static_cast<pa_threaded_mainloop *>(m_mainloop));
        m_mainloop = nullptr;
    }
}

void CallAudioRouter::start()
{
    m_routedIndex = PA_INVALID_INDEX;
    m_retryTimer->setInterval(PROBE_INTERVAL_MS);
    ensurePulseConnection();
    // The stream is rarely there this early, but if it is, finish the job here —
    // otherwise the watchdog below would see no change and never set the port.
    if (routeWebrtcToCallSink() && m_routedIndex != PA_INVALID_INDEX) {
        setSpeakerphone(m_speakerOn);
        m_retryTimer->setInterval(WATCHDOG_INTERVAL_MS);
    }
    m_retryTimer->start();
}

void CallAudioRouter::ensurePulseConnection()
{
    if (m_context) {
        return;
    }
    pa_threaded_mainloop *ml = pa_threaded_mainloop_new();
    if (!ml) {
        return;
    }
    pa_threaded_mainloop_start(ml);
    pa_threaded_mainloop_lock(ml);
    pa_context *ctx = pa_context_new(pa_threaded_mainloop_get_api(ml), "harbour-fernschreiber");
    pa_context_set_state_callback(ctx, &ctxStateCb, ml);
    pa_context_connect(ctx, nullptr, PA_CONTEXT_NOFLAGS, nullptr);
    for (;;) {
        const pa_context_state_t st = pa_context_get_state(ctx);
        if (st == PA_CONTEXT_READY) {
            break;
        }
        if (st == PA_CONTEXT_FAILED || st == PA_CONTEXT_TERMINATED) {
            pa_threaded_mainloop_unlock(ml);
            pa_context_unref(ctx);
            pa_threaded_mainloop_stop(ml);
            pa_threaded_mainloop_free(ml);
            return;
        }
        pa_threaded_mainloop_wait(ml);
    }
    SinkScan scan;
    scan.ml = ml;
    pa_operation *op = pa_context_get_sink_info_list(ctx, &sinkInfoCb, &scan);
    if (op) {
        while (pa_operation_get_state(op) == PA_OPERATION_RUNNING) {
            pa_threaded_mainloop_wait(ml);
        }
        pa_operation_unref(op);
    }
    pa_threaded_mainloop_unlock(ml);

    m_mainloop = ml;
    m_context = ctx;
    m_sink = scan.sink;
    m_speakerPort = scan.speaker;
    m_earpiecePort = scan.earpiece;
    if (m_sink.isEmpty()) {
        // Droid (Xperia) fallback names if port enumeration didn't match.
        m_sink = QStringLiteral("sink.primary_output");
        m_speakerPort = QStringLiteral("output-speaker");
        m_earpiecePort = QStringLiteral("output-earpiece");
    }
}

void CallAudioRouter::setSpeakerphone(bool on)
{
    m_speakerOn = on;
    ensurePulseConnection();
    pa_context *ctx = static_cast<pa_context *>(m_context);
    pa_threaded_mainloop *ml = static_cast<pa_threaded_mainloop *>(m_mainloop);
    if (!ctx || !ml || m_sink.isEmpty() || pa_context_get_state(ctx) != PA_CONTEXT_READY) {
        return;
    }
    const QString port = on ? m_speakerPort : m_earpiecePort;
    if (port.isEmpty()) {
        return;
    }
    pa_threaded_mainloop_lock(ml);
    pa_operation *op = pa_context_set_sink_port_by_name(ctx, m_sink.toUtf8().constData(),
                                                        port.toUtf8().constData(), nullptr, nullptr);
    if (op) {
        pa_operation_unref(op);
    }
    pa_threaded_mainloop_unlock(ml);
    routeWebrtcToCallSink();
}

bool CallAudioRouter::routeWebrtcToCallSink()
{
    ensurePulseConnection();
    pa_context *ctx = static_cast<pa_context *>(m_context);
    pa_threaded_mainloop *ml = static_cast<pa_threaded_mainloop *>(m_mainloop);
    if (!ctx || !ml || pa_context_get_state(ctx) != PA_CONTEXT_READY) {
        return false;
    }
    SinkInputScan scan;
    scan.ml = ml;
    pa_threaded_mainloop_lock(ml);
    pa_operation *op = pa_context_get_sink_input_info_list(ctx, &sinkInputInfoCb, &scan);
    if (op) {
        while (pa_operation_get_state(op) == PA_OPERATION_RUNNING) {
            pa_threaded_mainloop_wait(ml);
        }
        pa_operation_unref(op);
    }
    if (scan.found) {
        // Nothing to do while the stream we already routed is still there and audible.
        // A different index means tgcalls replaced the stream; a muted one means the
        // policy or a fresh stream muted it again.
        if (scan.index == m_routedIndex && !scan.mute) {
            pa_threaded_mainloop_unlock(ml);
            return true;
        }
        // Move the WebRTC playout onto the call sink (primary_output). The sink's
        // speaker/earpiece ports then route it like a real call; the port set in
        // setSpeakerphone() also wakes that sink from SUSPENDED so it plays.
        if (!m_sink.isEmpty()) {
            pa_operation *om = pa_context_move_sink_input_by_name(
                ctx, scan.index, m_sink.toUtf8().constData(), nullptr, nullptr);
            if (om) {
                pa_operation_unref(om);
            }
        }
        // Unmute (born muted on Halium).
        pa_operation *o2 = pa_context_set_sink_input_mute(ctx, scan.index, 0, nullptr, nullptr);
        if (o2) {
            pa_operation_unref(o2);
        }
        // flat-volumes couples the stream volume to the sink: save the sink's clean
        // volume once before boosting, so stop() can restore it after the call.
        if (!m_sinkVolumeSaved && !m_sink.isEmpty()) {
            SinkVolumeScan vscan;
            vscan.ml = ml;
            pa_operation *ov = pa_context_get_sink_info_by_name(
                ctx, m_sink.toUtf8().constData(), &sinkVolumeInfoCb, &vscan);
            if (ov) {
                while (pa_operation_get_state(ov) == PA_OPERATION_RUNNING) {
                    pa_threaded_mainloop_wait(ml);
                }
                pa_operation_unref(ov);
            }
            if (vscan.found) {
                m_savedSinkVolume = vscan.volume;
                m_sinkVolumeSaved = true;
            }
        }
        // Guard against the stream being born at 0% / -inf dB on some SFOS builds.
        pa_cvolume cv;
        pa_cvolume_set(&cv, scan.channels, (PA_VOLUME_NORM * 9) / 10);
        pa_operation *o3 = pa_context_set_sink_input_volume(ctx, scan.index, &cv, nullptr, nullptr);
        if (o3) {
            pa_operation_unref(o3);
        }
        m_routedIndex = scan.index;
    }
    pa_threaded_mainloop_unlock(ml);
    return scan.found;
}

void CallAudioRouter::stop()
{
    m_retryTimer->stop();
    m_retryTimer->setInterval(PROBE_INTERVAL_MS);
    m_routedIndex = PA_INVALID_INDEX;
    if (m_sinkVolumeSaved) {
        pa_context *ctx = static_cast<pa_context *>(m_context);
        pa_threaded_mainloop *ml = static_cast<pa_threaded_mainloop *>(m_mainloop);
        if (ctx && ml && !m_sink.isEmpty() && pa_context_get_state(ctx) == PA_CONTEXT_READY) {
            pa_threaded_mainloop_lock(ml);
            pa_operation *o = pa_context_set_sink_volume_by_name(
                ctx, m_sink.toUtf8().constData(), &m_savedSinkVolume, nullptr, nullptr);
            if (o) {
                pa_operation_unref(o);
            }
            pa_threaded_mainloop_unlock(ml);
        }
        m_sinkVolumeSaved = false;
    }
}
