/*
    Copyright (C) 2026 Fernschreiber contributors

    This file is part of Fernschreiber and is licensed under the GNU General
    Public License v3.0 (see LICENSE).

    tgcalls PlatformInterface implementation for Sailfish OS. This replaces
    tgcalls' FakeInterface / platform-specific interfaces and is the single
    entry point tgcalls resolves via CreatePlatformInterface().

    Concept adapted from RooTelegram's SailfishInterface (GPL-3.0,
    github.com/RootGPT-YouTube/RooTelegram-SailfishOS) and Yottagram, but
    re-implemented: this initial version is voice-first — video capture/render
    is added in a later step, so makeVideoSource/makeVideoCapturer return null.
*/
#ifndef FERNSCHREIBER_SAILFISHINTERFACE_H
#define FERNSCHREIBER_SAILFISHINTERFACE_H

#include "platform/PlatformInterface.h"

namespace fernschreiber {

class SailfishInterface : public tgcalls::PlatformInterface {
public:
    std::unique_ptr<webrtc::VideoEncoderFactory> makeVideoEncoderFactory(bool preferHardwareEncoding = false, bool isScreencast = false) override;
    std::unique_ptr<webrtc::VideoDecoderFactory> makeVideoDecoderFactory() override;
    bool supportsEncoding(const std::string &codecName) override;
    webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> makeVideoSource(rtc::Thread *signalingThread, rtc::Thread *workerThread) override;
    void adaptVideoSource(webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> videoSource, int width, int height, int fps) override;
    std::unique_ptr<tgcalls::VideoCapturerInterface> makeVideoCapturer(
        webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> source,
        std::string deviceId,
        std::function<void(tgcalls::VideoState)> stateUpdated,
        std::function<void(tgcalls::PlatformCaptureInfo)> captureInfoUpdated,
        std::shared_ptr<tgcalls::PlatformContext> platformContext,
        std::pair<int, int> &outResolution) override;
};

} // namespace fernschreiber

#endif // FERNSCHREIBER_SAILFISHINTERFACE_H
