/*
    Copyright (C) 2026 Fernschreiber contributors

    This file is part of Fernschreiber and is licensed under the GNU General
    Public License v3.0 (see LICENSE).

    Routes the WebRTC call audio on Sailfish/Halium. The incoming-audio stream
    ("WEBRTC VoiceEngine") is created MUTED on the media sink (deep_buffer); it
    must be moved to the call sink (primary_output, which carries the speaker /
    earpiece ports) and unmuted, or the remote party is inaudible. Uses an
    in-process libpulse connection because Sailjail blocks an external pactl but
    allows the app's own PulseAudio access. Adapted from RooTelegram (GPL-3.0,
    github.com/RootGPT-YouTube/RooTelegram-SailfishOS).
*/
#ifndef FERNSCHREIBER_CALLAUDIOROUTER_H
#define FERNSCHREIBER_CALLAUDIOROUTER_H

#include <QObject>
#include <QString>
#include <pulse/volume.h>

class QTimer;

class CallAudioRouter : public QObject
{
    Q_OBJECT
public:
    explicit CallAudioRouter(QObject *parent = nullptr);
    ~CallAudioRouter() override;

    // Begin routing: retries until the WebRTC playout stream appears, then moves
    // it to the call sink and unmutes it. Keeps watching for the rest of the call —
    // tgcalls recreates the playout stream on events like a camera switch, and the
    // replacement is born muted on the media sink again.
    void start();
    // Stop routing and restore the system sink volume.
    void stop();
    // Toggle speaker vs earpiece (ports of the single call sink).
    void setSpeakerphone(bool on);

private:
    void ensurePulseConnection();
    bool routeWebrtcToCallSink();

    QTimer *m_retryTimer;
    void *m_mainloop;   // pa_threaded_mainloop*
    void *m_context;    // pa_context*
    QString m_sink;
    QString m_speakerPort;
    QString m_earpiecePort;
    bool m_speakerOn;
    pa_cvolume m_savedSinkVolume;
    bool m_sinkVolumeSaved;
    quint32 m_routedIndex;   // sink input we last routed, PA_INVALID_INDEX if none
};

#endif // FERNSCHREIBER_CALLAUDIOROUTER_H
