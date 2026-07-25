/*
    Copyright (C) 2026 Fernschreiber contributors

    This file is part of Fernschreiber and is licensed under the GNU General
    Public License v3.0 (see LICENSE).

    Captures camera frames via QtMultimedia (headless viewfinder surface),
    converts them to WebRTC I420 (libyuv) and hands them to a callback. Adapted
    from RooTelegram's CallCameraGrabber (GPL-3.0,
    github.com/RootGPT-YouTube/RooTelegram-SailfishOS) — the device-specific
    pixel-format/rotation handling is kept as-is because it is hard-won on Halium.
*/
#ifndef FERNSCHREIBER_CALLCAMERAGRABBER_H
#define FERNSCHREIBER_CALLCAMERAGRABBER_H

#include <QObject>
#include <QPointer>
#include <QMutex>
#include <QVideoFrame>
#include <atomic>
#include <functional>
#include <utility>

class QCamera;
class QAbstractVideoSurface;
class QOrientationSensor;
namespace webrtc { class VideoFrame; }

namespace fernschreiber {

class CallCameraGrabber : public QObject
{
    Q_OBJECT
public:
    explicit CallCameraGrabber(QObject *parent = nullptr);
    ~CallCameraGrabber() override;

    void setFrameCallback(std::function<void(const webrtc::VideoFrame &)> cb);
    std::pair<int, int> resolution() const;

    // Called (queued) on the GUI thread — QCamera needs a Qt event loop.
    Q_INVOKABLE void start(bool front);
    Q_INVOKABLE void stop();

    // Called on the camera delivery thread by the viewfinder surface.
    void handleFrame(const QVideoFrame &frame);

private:
    // WebRTC frame rotation for the current device orientation + camera. Values
    // derived from LabCam's tested per-device rotation, +180 (WebRTC signals the
    // receiver how to rotate, which is the inverse of the capture rotation).
    static int rotationFor(int orientationReading, bool front);

    QPointer<QCamera> m_camera;
    QAbstractVideoSurface *m_surface;
    QOrientationSensor *m_orientationSensor;
    bool m_front;
    std::atomic<int> m_width;
    std::atomic<int> m_height;
    std::atomic<int> m_rotation;
    QMutex m_cbMutex;
    std::function<void(const webrtc::VideoFrame &)> m_cb;
};

} // namespace fernschreiber

#endif // FERNSCHREIBER_CALLCAMERAGRABBER_H
