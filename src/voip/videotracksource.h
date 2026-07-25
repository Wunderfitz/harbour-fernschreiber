/*
    Copyright (C) 2026 Fernschreiber contributors

    This file is part of Fernschreiber and is licensed under the GNU General
    Public License v3.0 (see LICENSE).

    Outgoing-video track source: a webrtc::VideoTrackSource exposing an
    rtc::VideoBroadcaster. The capturer (QtMultimedia camera) pushes I420 frames
    into sink()->OnFrame(); the broadcaster forwards them to the encoder. Kept
    in-tree to avoid tgcalls/tdesktop's desktop V4L2 dependencies. Adapted from
    RooTelegram (GPL-3.0).
*/
#ifndef FERNSCHREIBER_VIDEOTRACKSOURCE_H
#define FERNSCHREIBER_VIDEOTRACKSOURCE_H

#include "pc/video_track_source.h"
#include "api/video/video_sink_interface.h"
#include "media/base/video_broadcaster.h"

#include <memory>

namespace fernschreiber {

class VideoTrackSource : public webrtc::VideoTrackSource {
public:
    VideoTrackSource()
        : webrtc::VideoTrackSource(/*remote=*/false)
        , _broadcaster(std::make_shared<rtc::VideoBroadcaster>()) {}

    std::shared_ptr<rtc::VideoSinkInterface<webrtc::VideoFrame>> sink() {
        return _broadcaster;
    }

protected:
    rtc::VideoSourceInterface<webrtc::VideoFrame> *source() override {
        return _broadcaster.get();
    }

private:
    std::shared_ptr<rtc::VideoBroadcaster> _broadcaster;
};

} // namespace fernschreiber

#endif // FERNSCHREIBER_VIDEOTRACKSOURCE_H
