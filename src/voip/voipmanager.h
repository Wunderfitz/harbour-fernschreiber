/*
    Copyright (C) 2026 Fernschreiber contributors

    This file is part of Fernschreiber and is licensed under the GNU General
    Public License v3.0 (see LICENSE).

    VoipManager bridges TDLib's call signalling (via TDLibWrapper) to the tgcalls
    media engine: it builds the tgcalls Descriptor from callStateReady, owns the
    call Instance lifecycle, shuttles signalling data both ways, and (for video
    calls) owns the camera capture + the local/remote video renderers exposed to
    QML. The call protocol (incl. tgcalls library versions) is built here, keeping
    TDLibWrapper media-library agnostic.

    The Descriptor construction and the video wiring follow the logic proven in
    RooTelegram (GPL-3.0, github.com/RootGPT-YouTube/RooTelegram-SailfishOS) and
    Yottagram, re-implemented here with a narrower responsibility.
*/
#ifndef FERNSCHREIBER_VOIPMANAGER_H
#define FERNSCHREIBER_VOIPMANAGER_H

#include <QObject>
#include <QVariantMap>
#include <QByteArray>
#include <QStringList>
#include <QList>
#include <memory>
#include <vector>
#include <cstdint>

namespace tgcalls {
class Instance;
class VideoCaptureInterface;
}

class TDLibWrapper;
class CallAudioRouter;

namespace fernschreiber {
class VideoRenderer;
}

class VoipManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString callState READ callState NOTIFY callStateChanged)
    Q_PROPERTY(bool active READ active NOTIFY callStateChanged)
    Q_PROPERTY(qlonglong peerUserId READ peerUserId NOTIFY callChanged)
    Q_PROPERTY(bool isOutgoing READ isOutgoing NOTIFY callChanged)
    Q_PROPERTY(bool isVideo READ isVideo NOTIFY callChanged)
    Q_PROPERTY(QStringList emojis READ emojis NOTIFY emojisChanged)
    // Video: renderers exposed to QML VideoOutput, camera + remote-video state.
    Q_PROPERTY(QObject* remoteVideo READ remoteVideo CONSTANT)
    Q_PROPERTY(QObject* localVideo READ localVideo CONSTANT)
    Q_PROPERTY(bool frontCamera READ frontCamera NOTIFY frontCameraChanged)
    Q_PROPERTY(bool remoteVideoActive READ remoteVideoActive NOTIFY remoteVideoActiveChanged)

public:
    explicit VoipManager(TDLibWrapper *tdLibWrapper, QObject *parent = nullptr);
    ~VoipManager() override;

    QString callState() const { return m_callState; }
    bool active() const { return m_currentCallId != 0; }
    qlonglong peerUserId() const { return m_peerUserId; }
    bool isOutgoing() const { return m_isOutgoing; }
    bool isVideo() const { return m_isVideo; }
    QStringList emojis() const { return m_emojis; }
    QObject *remoteVideo() const;
    QObject *localVideo() const;
    bool frontCamera() const { return m_frontCamera; }
    bool remoteVideoActive() const { return m_remoteVideoActive; }

    // Start an outgoing call to a user. isVideo requests a video call.
    Q_INVOKABLE void startCall(qlonglong userId, bool isVideo);
    // Accept the current incoming call.
    Q_INVOKABLE void acceptCall();
    // Hang up / decline the current call.
    Q_INVOKABLE void hangUp();
    // Mute/unmute the local microphone on the active tgcalls instance.
    Q_INVOKABLE void setMicrophoneMuted(bool muted);
    // Video: switch front/back camera; enable/disable outgoing video.
    Q_INVOKABLE void switchCamera();
    Q_INVOKABLE void setVideoEnabled(bool enabled);

signals:
    void callStateChanged();
    void callChanged();
    void emojisChanged();
    void frontCameraChanged();
    void remoteVideoActiveChanged();

private slots:
    void handleCallUpdated(const QVariantMap &call);
    void handleCallSignalingDataReceived(qlonglong callId, const QByteArray &data);
    void setRemoteVideoActive(bool active);

private:
    // callProtocol object (incl. tgcalls library_versions) for createCall/acceptCall.
    QVariantMap buildProtocol() const;
    void ensureInstanceForReadyCall(const QVariantMap &callState);
    void stopInstance();
    void resetCall();
    static std::vector<uint8_t> toByteVector(const QByteArray &data);
    static QByteArray decodeTdlibBytes(const QString &data);

    TDLibWrapper *m_tdLibWrapper;
    CallAudioRouter *m_audioRouter;
    std::unique_ptr<tgcalls::Instance> m_instance;
    std::shared_ptr<tgcalls::VideoCaptureInterface> m_videoCapture;
    fernschreiber::VideoRenderer *m_remoteVideoRenderer;
    fernschreiber::VideoRenderer *m_localVideoRenderer;
    qlonglong m_currentCallId;
    qlonglong m_peerUserId;
    bool m_isOutgoing;
    bool m_isVideo;
    bool m_frontCamera;
    bool m_remoteVideoActive;
    QString m_callState;
    QStringList m_emojis;
    QList<QByteArray> m_pendingSignalingData;
};

#endif // FERNSCHREIBER_VOIPMANAGER_H
