/*
    Copyright (C) 2026 Fernschreiber contributors

    This file is part of Fernschreiber and is licensed under the GNU General
    Public License v3.0 (see LICENSE).

    tgcalls::VideoCapturerInterface for Sailfish: owns a CallCameraGrabber (a
    QObject living on the GUI thread, as QCamera requires a Qt event loop) and
    routes its I420 frames into the track source's broadcaster sink so the encoder
    sends them. Adapted from RooTelegram (GPL-3.0).
*/
#ifndef FERNSCHREIBER_VIDEOCAPTURER_H
#define FERNSCHREIBER_VIDEOCAPTURER_H

#include "VideoCapturerInterface.h"  // tgcalls
#include "api/media_stream_interface.h"
#include "api/video/video_sink_interface.h"
#include "api/scoped_refptr.h"

#include <memory>
#include <utility>
#include <functional>

namespace fernschreiber {

class CallCameraGrabber;

class VideoCapturer : public tgcalls::VideoCapturerInterface {
public:
    VideoCapturer(
        webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> source,
        std::shared_ptr<rtc::VideoSinkInterface<webrtc::VideoFrame>> sink,
        bool front,
        std::function<void(tgcalls::VideoState)> stateUpdated,
        std::pair<int, int> &outResolution);
    ~VideoCapturer() override;

    void setState(tgcalls::VideoState state) override;
    void setPreferredCaptureAspectRatio(float aspectRatio) override;
    void setUncroppedOutput(std::shared_ptr<rtc::VideoSinkInterface<webrtc::VideoFrame>> sink) override;
    int getRotation() override;

private:
    webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> _source;
    std::shared_ptr<rtc::VideoSinkInterface<webrtc::VideoFrame>> _sink;
    std::shared_ptr<rtc::VideoSinkInterface<webrtc::VideoFrame>> _uncroppedSink;
    CallCameraGrabber *_grabber;
    bool _front;
    tgcalls::VideoState _state;
    std::function<void(tgcalls::VideoState)> _stateUpdated;
};

} // namespace fernschreiber

#endif // FERNSCHREIBER_VIDEOCAPTURER_H
