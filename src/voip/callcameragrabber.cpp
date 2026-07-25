/*
    Copyright (C) 2026 Fernschreiber contributors

    This file is part of Fernschreiber and is licensed under the GNU General
    Public License v3.0 (see LICENSE). See callcameragrabber.h for the origin
    (adapted from RooTelegram, GPL-3.0). QtMultimedia camera -> I420 (libyuv).
*/
#include "callcameragrabber.h"

#include <QCamera>
#include <QCameraInfo>
#include <QCameraViewfinderSettings>
#include <QCameraExposure>
#include <QCameraImageProcessing>
#include <QAbstractVideoSurface>
#include <QOrientationSensor>
#include <QOrientationReading>
#include <QVideoSurfaceFormat>
#include <QVideoFrame>
#include <QList>
#include <QDebug>

#include "libyuv.h"
#include "api/video/video_frame.h"
#include "api/video/i420_buffer.h"
#include "rtc_base/time_utils.h"

namespace fernschreiber {

// Headless viewfinder surface: forwards every frame to the grabber.
class CallCameraSurface : public QAbstractVideoSurface
{
public:
    explicit CallCameraSurface(CallCameraGrabber *grabber, QObject *parent = nullptr)
        : QAbstractVideoSurface(parent), m_grabber(grabber) {}

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
        QAbstractVideoBuffer::HandleType /*handleType*/) const override
    {
        QList<QVideoFrame::PixelFormat> f;
        f << QVideoFrame::Format_NV21 << QVideoFrame::Format_NV12
          << QVideoFrame::Format_YUV420P << QVideoFrame::Format_YV12
          << QVideoFrame::Format_UYVY << QVideoFrame::Format_YUYV
          << QVideoFrame::Format_RGB32 << QVideoFrame::Format_ARGB32
          << QVideoFrame::Format_BGR32 << QVideoFrame::Format_BGRA32;
        return f;
    }

    bool present(const QVideoFrame &frame) override
    {
        if (m_grabber) {
            m_grabber->handleFrame(frame);
        }
        return true;
    }

private:
    CallCameraGrabber *m_grabber;
};

namespace {
// Plane pointer with bounds-check: multi-planar via bits(plane), else a verified
// contiguous offset — avoids crashing on the various Halium frame layouts.
const uint8_t *planePtr(QVideoFrame &f, int plane, int contiguousOffset,
                        int needBytes, int *outStride, int contiguousStride)
{
    if (plane < f.planeCount()) {
        *outStride = f.bytesPerLine(plane);
        return reinterpret_cast<const uint8_t *>(f.bits(plane));
    }
    const uchar *base = f.bits();
    if (!base || contiguousOffset + needBytes > f.mappedBytes()) {
        return nullptr;
    }
    *outStride = contiguousStride;
    return reinterpret_cast<const uint8_t *>(base) + contiguousOffset;
}
} // namespace

int CallCameraGrabber::rotationFor(int orientationReading, bool front)
{
    // Indexed by QOrientationReading::Orientation: 0=Undefined 1=TopUp 2=TopDown
    // 3=LeftUp 4=RightUp 5=FaceUp 6=FaceDown. Values are LabCam's per-device photo
    // rotations +180 (WebRTC rotation is the receiver-side inverse), tested on the
    // Xperia 10 III (front portrait 270, back portrait 90).
    static const int frontArr[7] = { 270, 270, 90, 180, 0, 270, 270 };
    static const int backArr[7]  = {  90,  90, 270, 180, 0,  90,  90 };
    if (orientationReading < 0 || orientationReading > 6) {
        orientationReading = 1;
    }
    return front ? frontArr[orientationReading] : backArr[orientationReading];
}

CallCameraGrabber::CallCameraGrabber(QObject *parent)
    : QObject(parent)
    , m_surface(nullptr)
    , m_orientationSensor(nullptr)
    , m_front(true)
    , m_width(0)
    , m_height(0)
    , m_rotation(270)
{
}

CallCameraGrabber::~CallCameraGrabber()
{
    stop();
}

void CallCameraGrabber::setFrameCallback(std::function<void(const webrtc::VideoFrame &)> cb)
{
    QMutexLocker lock(&m_cbMutex);
    m_cb = std::move(cb);
}

std::pair<int, int> CallCameraGrabber::resolution() const
{
    return { m_width.load(), m_height.load() };
}

void CallCameraGrabber::start(bool front)
{
    if (m_camera) {
        return;
    }
    m_front = front;
    // Follow the live device orientation so the transmitted image stays upright.
    if (!m_orientationSensor) {
        m_orientationSensor = new QOrientationSensor(this);
        connect(m_orientationSensor, &QOrientationSensor::readingChanged, this, [this]() {
            if (QOrientationReading *r = m_orientationSensor->reading()) {
                const int reading = int(r->orientation());
                // Only react to the four in-plane orientations (TopUp/TopDown/
                // LeftUp/RightUp = 1..4). Ignore Undefined/FaceUp/FaceDown so a
                // tilted phone during a call keeps the last sensible rotation.
                if (reading < 1 || reading > 4) {
                    return;
                }
                m_rotation.store(rotationFor(reading, m_front));
            }
        });
    }
    m_orientationSensor->start();
    QOrientationReading *initialReading = m_orientationSensor->reading();
    m_rotation.store(rotationFor(initialReading ? int(initialReading->orientation()) : 1, front));
    QCameraInfo chosen = QCameraInfo::defaultCamera();
    const QList<QCameraInfo> cams = QCameraInfo::availableCameras();
    const QCamera::Position wanted = front ? QCamera::FrontFace : QCamera::BackFace;
    for (const QCameraInfo &ci : cams) {
        if (ci.position() == wanted) { chosen = ci; break; }
    }
    if (chosen.isNull()) {
        qWarning() << "CallCameraGrabber: no camera available";
        return;
    }

    m_surface = new CallCameraSurface(this, this);
    m_camera = new QCamera(chosen, this);
    m_camera->setViewfinder(m_surface);
    m_camera->setCaptureMode(QCamera::CaptureVideo);
    // The camera defaults to a huge resolution (1920x1440 on the Xperia front cam),
    // which the software VP8 encoder can't sustain — it starves the incoming-video
    // decode and the remote picture vanishes. Pin a sane 720p (supported).
    {
        QCameraViewfinderSettings vf;
        vf.setResolution(1280, 720);
        m_camera->setViewfinderSettings(vf);
    }
    m_camera->start();
    // Tune the transmitted image: slight exposure lift (front cameras are often
    // dark) and a touch more saturation for punchier colour.
    if (m_camera->exposure()) {
        m_camera->exposure()->setExposureCompensation(0.7);
    }
    if (m_camera->imageProcessing()) {
        // Saturation range is -1.0..+1.0 (0 = default); +0.1 ≈ 10% more colour.
        m_camera->imageProcessing()->setSaturation(0.1);
    }
}

void CallCameraGrabber::stop()
{
    if (m_orientationSensor) {
        m_orientationSensor->stop();
    }
    if (m_camera) {
        m_camera->stop();
        m_camera->deleteLater();
        m_camera.clear();
    }
    if (m_surface) {
        m_surface->deleteLater();
        m_surface = nullptr;
    }
}

void CallCameraGrabber::handleFrame(const QVideoFrame &frame)
{
    // Only CPU-mappable frames (true on Xperia/Halium).
    if (frame.handleType() != QAbstractVideoBuffer::NoHandle) {
        return;
    }
    const int w = frame.width();
    const int h = frame.height();
    if (w <= 0 || h <= 0) {
        return;
    }
    QVideoFrame f(frame);
    if (!f.map(QAbstractVideoBuffer::ReadOnly)) {
        return;
    }

    auto i420 = webrtc::I420Buffer::Create(w, h);
    int sy = 0;
    const uint8_t *y = planePtr(f, 0, 0, f.bytesPerLine() * h, &sy, f.bytesPerLine());
    int rc = -1;
    if (y) {
        const QVideoFrame::PixelFormat fmt = f.pixelFormat();
        switch (fmt) {
        case QVideoFrame::Format_NV21:
        case QVideoFrame::Format_NV12: {
            int sc = 0;
            const uint8_t *c = planePtr(f, 1, sy * h, sy * (h / 2), &sc, sy);
            if (c) {
                rc = (fmt == QVideoFrame::Format_NV21)
                     ? libyuv::NV21ToI420(y, sy, c, sc,
                           i420->MutableDataY(), i420->StrideY(), i420->MutableDataU(), i420->StrideU(),
                           i420->MutableDataV(), i420->StrideV(), w, h)
                     : libyuv::NV12ToI420(y, sy, c, sc,
                           i420->MutableDataY(), i420->StrideY(), i420->MutableDataU(), i420->StrideU(),
                           i420->MutableDataV(), i420->StrideV(), w, h);
            }
            break;
        }
        case QVideoFrame::Format_YUV420P:
        case QVideoFrame::Format_YV12: {
            int s1 = 0, s2 = 0;
            const uint8_t *p1 = planePtr(f, 1, sy * h, (sy / 2) * (h / 2), &s1, sy / 2);
            const uint8_t *p2 = planePtr(f, 2, sy * h + (sy / 2) * (h / 2), (sy / 2) * (h / 2), &s2, sy / 2);
            if (p1 && p2) {
                const uint8_t *u = (fmt == QVideoFrame::Format_YV12) ? p2 : p1;
                const uint8_t *v = (fmt == QVideoFrame::Format_YV12) ? p1 : p2;
                const int su = (fmt == QVideoFrame::Format_YV12) ? s2 : s1;
                const int sv = (fmt == QVideoFrame::Format_YV12) ? s1 : s2;
                rc = libyuv::I420Copy(y, sy, u, su, v, sv,
                         i420->MutableDataY(), i420->StrideY(), i420->MutableDataU(), i420->StrideU(),
                         i420->MutableDataV(), i420->StrideV(), w, h);
            }
            break;
        }
        case QVideoFrame::Format_YUYV:
            rc = libyuv::YUY2ToI420(y, sy, i420->MutableDataY(), i420->StrideY(),
                     i420->MutableDataU(), i420->StrideU(), i420->MutableDataV(), i420->StrideV(), w, h);
            break;
        case QVideoFrame::Format_UYVY:
            rc = libyuv::UYVYToI420(y, sy, i420->MutableDataY(), i420->StrideY(),
                     i420->MutableDataU(), i420->StrideU(), i420->MutableDataV(), i420->StrideV(), w, h);
            break;
        case QVideoFrame::Format_RGB32:
        case QVideoFrame::Format_ARGB32:
        case QVideoFrame::Format_BGR32:
        case QVideoFrame::Format_BGRA32:
            rc = libyuv::ARGBToI420(y, sy, i420->MutableDataY(), i420->StrideY(),
                     i420->MutableDataU(), i420->StrideU(), i420->MutableDataV(), i420->StrideV(), w, h);
            break;
        default:
            break;
        }
    }
    f.unmap();
    if (rc != 0) {
        return;
    }

    m_width.store(w);
    m_height.store(h);

    // Camera-dependent rotation (270 front / 90 back); receiver + local PiP
    // straighten the frame.
    webrtc::VideoFrame videoFrame = webrtc::VideoFrame::Builder()
            .set_video_frame_buffer(i420)
            .set_rotation(static_cast<webrtc::VideoRotation>(m_rotation.load()))
            .set_timestamp_us(rtc::TimeMicros())
            .build();

    QMutexLocker lock(&m_cbMutex);
    if (m_cb) {
        m_cb(videoFrame);
    }
}

} // namespace fernschreiber
