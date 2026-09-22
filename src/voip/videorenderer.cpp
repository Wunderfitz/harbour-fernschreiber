/*
    Copyright (C) 2026 Fernschreiber contributors

    This file is part of Fernschreiber and is licensed under the GNU General
    Public License v3.0 (see LICENSE). Adapted from RooTelegram (GPL-3.0).
*/
#include "videorenderer.h"

#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QVideoFrame>
#include <QPointer>
#include <QMetaObject>
#include <QTransform>

#include "api/video/video_frame.h"
#include "api/video/video_sink_interface.h"
#include "api/video/i420_buffer.h"
#include "libyuv.h"

#include <QDebug>

namespace fernschreiber {

// webrtc sink: receives frames (webrtc thread), converts I420->RGB and marshals
// to the renderer (GUI thread). QPointer so a dead renderer doesn't crash.
class VideoRenderer::SinkBridge : public rtc::VideoSinkInterface<webrtc::VideoFrame>
{
public:
    explicit SinkBridge(VideoRenderer *renderer) : m_renderer(renderer) {}

    void OnFrame(const webrtc::VideoFrame &frame) override
    {
        webrtc::scoped_refptr<webrtc::I420BufferInterface> buf = frame.video_frame_buffer()->ToI420();
        if (!buf) {
            return;
        }
        const int w = buf->width();
        const int h = buf->height();
        if (w <= 0 || h <= 0) {
            return;
        }
        QImage img(w, h, QImage::Format_RGB32);
        libyuv::I420ToARGB(buf->DataY(), buf->StrideY(),
                           buf->DataU(), buf->StrideU(),
                           buf->DataV(), buf->StrideV(),
                           img.bits(), img.bytesPerLine(), w, h);
        // Apply rotation: a fixed override (local self-view) if set, else the
        // rotation signalled by the frame (remote / transmitted orientation).
        const int rot = (m_renderer && m_renderer->m_fixedRotation >= 0)
                ? m_renderer->m_fixedRotation
                : int(frame.rotation());
        if (rot != 0) {
            QTransform t;
            t.rotate(rot);
            img = img.transformed(t);
        }
        if (m_renderer) {
            QMetaObject::invokeMethod(m_renderer, "presentImage", Qt::QueuedConnection,
                                      Q_ARG(QImage, img));
        }
    }

private:
    QPointer<VideoRenderer> m_renderer;
};

VideoRenderer::VideoRenderer(QObject *parent)
    : QObject(parent)
    , m_surface(nullptr)
    , m_hasFrame(false)
{
}

VideoRenderer::~VideoRenderer()
{
    reset();
}

void VideoRenderer::setVideoSurface(QAbstractVideoSurface *surface)
{
    if (m_surface == surface) {
        return;
    }
    if (m_surface && m_surface->isActive()) {
        m_surface->stop();
    }
    m_surface = surface;
    m_size = QSize();
    emit videoSurfaceChanged();
}

std::shared_ptr<rtc::VideoSinkInterface<webrtc::VideoFrame>> VideoRenderer::sink()
{
    if (!m_sink) {
        m_sink = std::make_shared<SinkBridge>(this);
    }
    return m_sink;
}

void VideoRenderer::reset()
{
    m_sink.reset();
    if (m_surface && m_surface->isActive()) {
        m_surface->stop();
    }
    m_size = QSize();
    if (m_hasFrame) {
        m_hasFrame = false;
        emit hasFrameChanged();
    }
}

void VideoRenderer::presentImage(const QImage &image)
{
    if (!m_surface || image.isNull()) {
        return;
    }
    if (!m_surface->isActive() || m_size != image.size()) {
        if (m_surface->isActive()) {
            m_surface->stop();
        }
        QVideoSurfaceFormat fmt(image.size(), QVideoFrame::Format_RGB32);
        m_surface->start(fmt);
        m_size = image.size();
    }
    m_surface->present(QVideoFrame(image));
    if (!m_hasFrame) {
        m_hasFrame = true;
        emit hasFrameChanged();
    }
}

} // namespace fernschreiber
