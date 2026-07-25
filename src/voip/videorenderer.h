/*
    Copyright (C) 2026 Fernschreiber contributors

    This file is part of Fernschreiber and is licensed under the GNU General
    Public License v3.0 (see LICENSE).

    Renders one video stream (remote or local preview) into QML. It is a video
    producer (exposes videoSurface -> QML VideoOutput) and provides a webrtc sink
    (rtc::VideoSinkInterface) to hook into tgcalls (setIncomingVideoOutput for the
    remote, VideoCapturer::setUncroppedOutput for the local preview). Incoming
    I420 frames are converted to RGB and presented on the GUI thread. Adapted from
    RooTelegram's CallVideoRenderer (GPL-3.0).
*/
#ifndef FERNSCHREIBER_VIDEORENDERER_H
#define FERNSCHREIBER_VIDEORENDERER_H

#include <QObject>
#include <QImage>
#include <QSize>
#include <memory>

class QAbstractVideoSurface;

namespace rtc {
template <typename T> class VideoSinkInterface;
}
namespace webrtc { class VideoFrame; }

namespace fernschreiber {

class VideoRenderer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractVideoSurface* videoSurface READ videoSurface WRITE setVideoSurface NOTIFY videoSurfaceChanged)
    Q_PROPERTY(bool hasFrame READ hasFrame NOTIFY hasFrameChanged)

public:
    explicit VideoRenderer(QObject *parent = nullptr);
    ~VideoRenderer() override;

    QAbstractVideoSurface *videoSurface() const { return m_surface; }
    void setVideoSurface(QAbstractVideoSurface *surface);
    bool hasFrame() const { return m_hasFrame; }

    // webrtc sink to pass to tgcalls (shared: kept alive by the caller).
    std::shared_ptr<rtc::VideoSinkInterface<webrtc::VideoFrame>> sink();
    // Clear state at call end: stop the surface, hasFrame=false.
    void reset();
    // Override the per-frame rotation with a fixed value (>=0), or -1 to honour
    // frame.rotation(). Used for the local self-view: the app UI already rotates
    // with the device, so applying the frame's device-dependent rotation too would
    // rotate the preview twice.
    void setFixedRotation(int degrees) { m_fixedRotation = degrees; }

signals:
    void videoSurfaceChanged();
    void hasFrameChanged();

private slots:
    void presentImage(const QImage &image);

private:
    class SinkBridge;
    QAbstractVideoSurface *m_surface;
    std::shared_ptr<SinkBridge> m_sink;
    QSize m_size;
    bool m_hasFrame;
    int m_fixedRotation = -1;
};

} // namespace fernschreiber

#endif // FERNSCHREIBER_VIDEORENDERER_H
