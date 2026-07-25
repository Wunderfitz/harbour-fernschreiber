/*
    Copyright (C) 2026 Fernschreiber contributors

    This file is part of Fernschreiber and is licensed under the GNU General
    Public License v3.0 (see LICENSE).

    See sailfishinterface.h. tgcalls platform interface for SFOS: codec factories
    from tg_owt's builtin (VP8/VP9, H264 not offered), and camera capture via
    QtMultimedia (VideoCapturer + VideoTrackSource). Video wiring adapted from
    RooTelegram (GPL-3.0).
*/
#include "sailfishinterface.h"
#include "videotracksource.h"
#include "videocapturer.h"

// Asymmetric codecs: ENCODE VP8/VP9 only (consistent with supportsEncoding, and
// avoids the struggling OpenH264 encoder), but DECODE everything the builtin
// factory offers (incl. H264/H265) so official Telegram clients — which prefer
// H264 — are received reliably.
#include "api/video_codecs/video_encoder_factory_template.h"
#include "api/video_codecs/video_encoder_factory_template_libvpx_vp8_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_libvpx_vp9_adapter.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "tgcalls/VideoCaptureInterface.h"
#include "pc/video_track_source_proxy.h"
#include "rtc_base/ref_counted_object.h"

namespace fernschreiber {

namespace {
// Recover the broadcaster sink from the proxied track source so the capturer
// can push camera frames into it.
std::shared_ptr<rtc::VideoSinkInterface<webrtc::VideoFrame>> GetSink(
        const webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> &nativeSource)
{
    auto *proxy = static_cast<webrtc::VideoTrackSourceProxy *>(nativeSource.get());
    auto *internal = static_cast<VideoTrackSource *>(proxy->internal());
    return internal->sink();
}
} // namespace

std::unique_ptr<webrtc::VideoEncoderFactory> SailfishInterface::makeVideoEncoderFactory(bool /*preferHardwareEncoding*/, bool /*isScreencast*/)
{
    return std::make_unique<webrtc::VideoEncoderFactoryTemplate<
        webrtc::LibvpxVp8EncoderTemplateAdapter,
        webrtc::LibvpxVp9EncoderTemplateAdapter>>();
}

std::unique_ptr<webrtc::VideoDecoderFactory> SailfishInterface::makeVideoDecoderFactory()
{
    return webrtc::CreateBuiltinVideoDecoderFactory();
}

bool SailfishInterface::supportsEncoding(const std::string &codecName)
{
    // VP8/VP9 via libvpx; native H264 is a stub, so not offered.
    return codecName == "VP8" || codecName == "VP9";
}

webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> SailfishInterface::makeVideoSource(rtc::Thread *signalingThread, rtc::Thread *workerThread)
{
    const auto trackSource = webrtc::scoped_refptr<VideoTrackSource>(
        new rtc::RefCountedObject<VideoTrackSource>());
    return trackSource
        ? webrtc::VideoTrackSourceProxy::Create(signalingThread, workerThread, trackSource)
        : nullptr;
}

void SailfishInterface::adaptVideoSource(webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> /*videoSource*/, int /*width*/, int /*height*/, int /*fps*/)
{
}

std::unique_ptr<tgcalls::VideoCapturerInterface> SailfishInterface::makeVideoCapturer(
    webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> source,
    std::string deviceId,
    std::function<void(tgcalls::VideoState)> stateUpdated,
    std::function<void(tgcalls::PlatformCaptureInfo)> /*captureInfoUpdated*/,
    std::shared_ptr<tgcalls::PlatformContext> /*platformContext*/,
    std::pair<int, int> &outResolution)
{
    // deviceId "back" -> rear camera; otherwise front (video-call default).
    const bool front = (deviceId != "back");
    return std::unique_ptr<tgcalls::VideoCapturerInterface>(
        new VideoCapturer(source, GetSink(source), front, std::move(stateUpdated), outResolution));
}

} // namespace fernschreiber

namespace tgcalls {

std::unique_ptr<PlatformInterface> CreatePlatformInterface()
{
    return std::make_unique<fernschreiber::SailfishInterface>();
}

} // namespace tgcalls
