/*
    Copyright (C) 2026 Fernschreiber contributors

    This file is part of Fernschreiber and is licensed under the GNU General
    Public License v3.0 (see LICENSE). Adapted from RooTelegram (GPL-3.0).
*/
#include "videocapturer.h"
#include "callcameragrabber.h"

#include "VideoCaptureInterface.h"  // tgcalls::VideoState enum values

#include <QCoreApplication>
#include <QMetaObject>
#include <QThread>

namespace fernschreiber {

VideoCapturer::VideoCapturer(
        webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> source,
        std::shared_ptr<rtc::VideoSinkInterface<webrtc::VideoFrame>> sink,
        bool front,
        std::function<void(tgcalls::VideoState)> stateUpdated,
        std::pair<int, int> &outResolution)
    : _source(source)
    , _sink(sink)
    , _grabber(new CallCameraGrabber())
    , _front(front)
    , _state(tgcalls::VideoState::Inactive)
    , _stateUpdated(std::move(stateUpdated))
{
    // Captured frames (on the camera delivery thread) are pushed into the track
    // source's broadcaster -> encoder. The sink is thread-safe.
    auto sinkCopy = _sink;
    _grabber->setFrameCallback([sinkCopy](const webrtc::VideoFrame &frame) {
        if (sinkCopy) {
            sinkCopy->OnFrame(frame);
        }
    });

    // The grabber must live on the GUI thread (QCamera needs a Qt event loop):
    // it is constructed here (a tgcalls media thread) and moved to qApp's thread.
    if (QCoreApplication::instance()) {
        _grabber->moveToThread(QCoreApplication::instance()->thread());
    }

    // Declared initial resolution (camera is 4:3); the real one arrives with frames.
    outResolution = { 1280, 960 };
}

VideoCapturer::~VideoCapturer()
{
    if (_grabber) {
        _grabber->setFrameCallback(nullptr);
        QMetaObject::invokeMethod(_grabber, "stop", Qt::QueuedConnection);
        _grabber->deleteLater(); // destroyed on its own (GUI) thread
        _grabber = nullptr;
    }
}

void VideoCapturer::setState(tgcalls::VideoState state)
{
    _state = state;
    if (!_grabber) {
        return;
    }
    if (state == tgcalls::VideoState::Active) {
        QMetaObject::invokeMethod(_grabber, "start", Qt::QueuedConnection,
                                  Q_ARG(bool, _front));
    } else {
        QMetaObject::invokeMethod(_grabber, "stop", Qt::QueuedConnection);
    }
    if (_stateUpdated) {
        _stateUpdated(state);
    }
}

void VideoCapturer::setPreferredCaptureAspectRatio(float /*aspectRatio*/)
{
}

void VideoCapturer::setUncroppedOutput(
        std::shared_ptr<rtc::VideoSinkInterface<webrtc::VideoFrame>> sink)
{
    // Extra sink attached directly to the source (local preview).
    if (_uncroppedSink) {
        _source->RemoveSink(_uncroppedSink.get());
    }
    _uncroppedSink = sink;
    if (_uncroppedSink) {
        _source->AddOrUpdateSink(_uncroppedSink.get(), rtc::VideoSinkWants());
    }
}

int VideoCapturer::getRotation()
{
    return 0;
}

} // namespace fernschreiber
