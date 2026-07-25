/*
    Copyright (C) 2026 Fernschreiber contributors

    This file is part of Fernschreiber and is licensed under the GNU General
    Public License v3.0 (see LICENSE). See voipmanager.h for the design and the
    attribution of the Descriptor logic (RooTelegram / Yottagram, GPL-3.0).
*/
#include "voipmanager.h"
#include "tdlibwrapper.h"

#define DEBUG_MODULE VoipManager
#include "debuglog.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <thread>
#include <QMetaObject>
#include <QVariantList>

#include <tgcalls/Instance.h>
#include <tgcalls/InstanceImpl.h>
#include <tgcalls/StaticThreads.h>
#include <tgcalls/VideoCaptureInterface.h>
#include <tgcalls/v2/InstanceV2Impl.h>
#include <tgcalls/v2/InstanceV2ReferenceImpl.h>

#include "videorenderer.h"
#include "callaudiorouter.h"

namespace {
// Self-registering call implementations; Meta::Create() dispatches by version.
const auto RegisterLegacy = tgcalls::Register<tgcalls::InstanceImpl>();
const auto RegisterV2 = tgcalls::Register<tgcalls::InstanceV2Impl>();
const auto RegisterV2Reference = tgcalls::Register<tgcalls::InstanceV2ReferenceImpl>();
}

VoipManager::VoipManager(TDLibWrapper *tdLibWrapper, QObject *parent)
    : QObject(parent)
    , m_tdLibWrapper(tdLibWrapper)
    , m_audioRouter(new CallAudioRouter(this))
    , m_remoteVideoRenderer(new fernschreiber::VideoRenderer(this))
    , m_localVideoRenderer(new fernschreiber::VideoRenderer(this))
    // (objectNames set below for diagnostic frame logging)
    , m_currentCallId(0)
    , m_peerUserId(0)
    , m_isOutgoing(false)
    , m_isVideo(false)
    , m_frontCamera(true)
    , m_remoteVideoActive(false)
{
    Q_UNUSED(RegisterLegacy)
    Q_UNUSED(RegisterV2)
    Q_UNUSED(RegisterV2Reference)
    m_remoteVideoRenderer->setObjectName(QStringLiteral("remote"));
    m_localVideoRenderer->setObjectName(QStringLiteral("local"));
    if (m_tdLibWrapper) {
        connect(m_tdLibWrapper, &TDLibWrapper::callUpdated, this, &VoipManager::handleCallUpdated);
        connect(m_tdLibWrapper, &TDLibWrapper::callSignalingDataReceived, this, &VoipManager::handleCallSignalingDataReceived);
    }
}

QObject *VoipManager::remoteVideo() const
{
    return m_remoteVideoRenderer;
}

QObject *VoipManager::localVideo() const
{
    return m_localVideoRenderer;
}

VoipManager::~VoipManager()
{
    stopInstance();
}

QVariantMap VoipManager::buildProtocol() const
{
    QVariantMap protocol;
    protocol.insert("@type", "callProtocol");
    protocol.insert("udp_p2p", true);
    protocol.insert("udp_reflector", true);
    protocol.insert("min_layer", 65);
    protocol.insert("max_layer", 92);
    std::vector<std::string> versions = tgcalls::Meta::Versions();
    std::reverse(versions.begin(), versions.end());
    QVariantList versionList;
    for (std::vector<std::string>::const_iterator it = versions.cbegin(); it != versions.cend(); ++it) {
        versionList.append(QString::fromStdString(*it));
    }
    protocol.insert("library_versions", versionList);
    return protocol;
}

void VoipManager::startCall(qlonglong userId, bool isVideo)
{
    if (m_currentCallId != 0 || !m_tdLibWrapper) {
        return;
    }
    LOG("Starting call to user" << userId << "video:" << isVideo);
    m_tdLibWrapper->createCall(userId, buildProtocol(), isVideo);
}

void VoipManager::acceptCall()
{
    if (m_currentCallId == 0 || !m_tdLibWrapper) {
        return;
    }
    LOG("Accepting call" << m_currentCallId);
    m_tdLibWrapper->acceptCall(m_currentCallId, buildProtocol());
}

void VoipManager::hangUp()
{
    if (m_currentCallId == 0 || !m_tdLibWrapper) {
        return;
    }
    LOG("Hanging up call" << m_currentCallId);
    m_tdLibWrapper->discardCall(m_currentCallId, false, 0, m_isVideo, 0);
}

void VoipManager::setMicrophoneMuted(bool muted)
{
    if (m_instance) {
        m_instance->setMuteMicrophone(muted);
    }
}

void VoipManager::switchCamera()
{
    if (!m_videoCapture) {
        return;
    }
    m_frontCamera = !m_frontCamera;
    // deviceId "" -> front, "back" -> rear (see SailfishInterface::makeVideoCapturer).
    m_videoCapture->switchToDevice(m_frontCamera ? std::string() : std::string("back"), false);
    emit frontCameraChanged();
}

void VoipManager::setVideoEnabled(bool enabled)
{
    if (!m_videoCapture || !m_instance) {
        return;
    }
    if (enabled) {
        m_videoCapture->setState(tgcalls::VideoState::Active);
        m_instance->setVideoCapture(m_videoCapture);
    } else {
        // Detach so tgcalls signals the peer that video is off (instead of a frozen
        // last frame), and stop the camera.
        m_instance->setVideoCapture(nullptr);
        m_videoCapture->setState(tgcalls::VideoState::Inactive);
    }
}

void VoipManager::setRemoteVideoActive(bool active)
{
    if (m_remoteVideoActive != active) {
        m_remoteVideoActive = active;
        emit remoteVideoActiveChanged();
    }
    // Do NOT reset the remote renderer here: reset() destroys the webrtc sink that
    // tgcalls holds (as a weak_ptr) via setIncomingVideoOutput. An early
    // remoteMediaStateUpdated(Inactive) would then permanently kill incoming video.
    // The sink is only torn down at call end (stopInstance).
}

void VoipManager::handleCallUpdated(const QVariantMap &call)
{
    const qlonglong callId = call.value("id").toLongLong();
    if (callId <= 0) {
        return;
    }
    m_currentCallId = callId;
    m_peerUserId = call.value("user_id").toLongLong();
    m_isOutgoing = call.value("is_outgoing").toBool();
    m_isVideo = call.value("is_video").toBool();
    emit callChanged();

    const QVariantMap state = call.value("state").toMap();
    const QString stateType = state.value("@type").toString();
    LOG("Call updated" << callId << stateType << "outgoing:" << m_isOutgoing << "video:" << m_isVideo);

    if (m_callState != stateType) {
        m_callState = stateType;
        emit callStateChanged();
    }

    if (stateType == "callStateReady") {
        ensureInstanceForReadyCall(state);
    } else if (stateType == "callStateDiscarded" || stateType == "callStateError") {
        stopInstance();
        resetCall();
    }
}

void VoipManager::handleCallSignalingDataReceived(qlonglong callId, const QByteArray &data)
{
    if (callId <= 0 || data.isEmpty()) {
        return;
    }
    if (m_currentCallId != 0 && callId != m_currentCallId) {
        return;
    }
    if (m_instance) {
        m_instance->receiveSignalingData(toByteVector(data));
    } else {
        m_pendingSignalingData.append(data);
    }
}

void VoipManager::ensureInstanceForReadyCall(const QVariantMap &callState)
{
    if (m_instance || m_currentCallId <= 0) {
        return;
    }

    // SAS verification emojis for the UI.
    const QVariantList emojiList = callState.value("emojis").toList();
    m_emojis.clear();
    for (QVariantList::const_iterator it = emojiList.cbegin(); it != emojiList.cend(); ++it) {
        m_emojis.append(it->toString());
    }
    emit emojisChanged();

    const QVariantMap protocol = callState.value("protocol").toMap();
    const QVariantList remoteVersions = protocol.value("library_versions").toList();
    std::vector<std::string> localVersions = tgcalls::Meta::Versions();
    std::reverse(localVersions.begin(), localVersions.end());

    QString selectedVersion;
    for (QVariantList::const_iterator it = remoteVersions.cbegin(); it != remoteVersions.cend() && selectedVersion.isEmpty(); ++it) {
        const QString remoteVersion = it->toString();
        for (std::vector<std::string>::const_iterator lv = localVersions.cbegin(); lv != localVersions.cend(); ++lv) {
            if (remoteVersion == QString::fromStdString(*lv)) {
                selectedVersion = remoteVersion;
                break;
            }
        }
    }
    if (selectedVersion.isEmpty() && !localVersions.empty()) {
        selectedVersion = QString::fromStdString(localVersions.front());
    }
    if (selectedVersion.isEmpty()) {
        WARN("No common call protocol version");
        return;
    }

    const QByteArray keyData = decodeTdlibBytes(callState.value("encryption_key").toString());
    if (keyData.isEmpty()) {
        WARN("Missing call encryption key");
        return;
    }
    std::shared_ptr<std::array<uint8_t, tgcalls::EncryptionKey::kSize>> encryptionKey =
            std::make_shared<std::array<uint8_t, tgcalls::EncryptionKey::kSize>>();
    encryptionKey->fill(0);
    std::memcpy(encryptionKey->data(), keyData.constData(),
                std::min<int>(keyData.size(), int(tgcalls::EncryptionKey::kSize)));

    tgcalls::Descriptor descriptor{
        selectedVersion.toStdString(),
        tgcalls::Config(),
        tgcalls::PersistentState(),
        std::vector<tgcalls::Endpoint>(),
        std::unique_ptr<tgcalls::Proxy>(),
        std::vector<tgcalls::RtcServer>(),
        tgcalls::NetworkType::WiFi,
        tgcalls::EncryptionKey(encryptionKey, m_isOutgoing)
    };
    descriptor.config.initializationTimeout = 30.0;
    descriptor.config.receiveTimeout = 20.0;
    descriptor.config.enableP2P = callState.contains("allow_p2p") ? callState.value("allow_p2p").toBool() : true;
    descriptor.config.allowTCP = true;
    descriptor.config.enableStunMarking = true;
    descriptor.config.enableAEC = true;
    descriptor.config.enableNS = true;
    descriptor.config.enableAGC = true;
    descriptor.config.maxApiLayer = protocol.value("max_layer").toInt();
    if (descriptor.config.maxApiLayer <= 0) {
        descriptor.config.maxApiLayer = tgcalls::Meta::MaxLayer();
    }

    // Video calls: create the camera capture (SailfishInterface -> QtMultimedia ->
    // I420 -> VP8/VP9 encoder) and hand it to tgcalls. Audio path is unchanged.
    if (m_isVideo) {
        m_frontCamera = true;
        emit frontCameraChanged();
        m_remoteVideoActive = false;
        emit remoteVideoActiveChanged();
        m_videoCapture = tgcalls::VideoCaptureInterface::Create(
            tgcalls::StaticThreads::getThreads(), std::string(), false, nullptr);
        descriptor.videoCapture = m_videoCapture;
    }

    // Remote media state -> the peer turned their camera on/off (QML shows the
    // remote video or an avatar/placeholder accordingly).
    descriptor.remoteMediaStateUpdated = [this](tgcalls::AudioState /*audioState*/, tgcalls::VideoState videoState) {
        const bool activeVideo = (videoState == tgcalls::VideoState::Active);
        QMetaObject::invokeMethod(this, "setRemoteVideoActive", Qt::QueuedConnection,
                                  Q_ARG(bool, activeVideo));
    };

    // Relay/WebRTC servers live under "servers" (NOT "connections"). V2 instances
    // ignore descriptor.endpoints, so a reflector must ALSO be exposed as an
    // RtcServer with login="reflector" and password=hex(peerTag) (parsed back as
    // hex by ReflectorPort) — without it, mobile-NAT calls fail to connect.
    const QVariantList servers = callState.value("servers").toList();
    for (QVariantList::const_iterator it = servers.cbegin(); it != servers.cend(); ++it) {
        const QVariantMap server = it->toMap();
        const QVariantMap type = server.value("type").toMap();
        const QString typeName = type.value("@type").toString();
        const QString host = !server.value("ip_address").toString().isEmpty()
                ? server.value("ip_address").toString()
                : server.value("ipv6_address").toString();
        const uint16_t port = static_cast<uint16_t>(server.value("port").toUInt());

        if (typeName == "callServerTypeTelegramReflector") {
            const QByteArray peerTag = decodeTdlibBytes(type.value("peer_tag").toString());
            const bool isTcp = type.value("is_tcp").toBool();

            tgcalls::Endpoint endpoint;
            endpoint.endpointId = server.value("id").toLongLong();
            endpoint.host = tgcalls::EndpointHost{
                server.value("ip_address").toString().toStdString(),
                server.value("ipv6_address").toString().toStdString()
            };
            endpoint.port = port;
            endpoint.type = isTcp ? tgcalls::EndpointType::TcpRelay : tgcalls::EndpointType::UdpRelay;
            if (peerTag.size() >= 16) {
                std::memcpy(endpoint.peerTag, peerTag.constData(), 16);
            }
            descriptor.endpoints.push_back(endpoint);

            if (!isTcp && peerTag.size() >= 16) {
                tgcalls::RtcServer reflector;
                reflector.id = static_cast<uint8_t>((descriptor.rtcServers.size() % 250) + 1);
                reflector.host = host.toStdString();
                reflector.port = port;
                reflector.login = "reflector";
                reflector.password = QString::fromLatin1(peerTag.toHex()).toStdString();
                reflector.isTurn = true;
                reflector.isTcp = false;
                descriptor.rtcServers.push_back(reflector);
            }
        } else if (typeName == "callServerTypeWebrtc") {
            const int serverId = server.value("id").toInt();
            const uint8_t rtcId = static_cast<uint8_t>(serverId < 0 ? 0 : (serverId > 255 ? 255 : serverId));
            if (type.value("supports_stun").toBool()) {
                tgcalls::RtcServer stun;
                stun.id = rtcId;
                stun.host = host.toStdString();
                stun.port = port;
                stun.isTurn = false;
                descriptor.rtcServers.push_back(stun);
            }
            if (type.value("supports_turn").toBool()) {
                tgcalls::RtcServer turn;
                turn.id = rtcId;
                turn.host = host.toStdString();
                turn.port = port;
                turn.login = type.value("username").toString().toStdString();
                turn.password = type.value("password").toString().toStdString();
                turn.isTurn = true;
                turn.isTcp = type.value("is_tcp").toBool();
                descriptor.rtcServers.push_back(turn);
            }
        }
    }

    descriptor.stateUpdated = [](tgcalls::State state) {
        LOG("tgcalls state" << static_cast<int>(state));  // 0=WaitInit 2=Established 3=Failed
    };
    const qlonglong callId = m_currentCallId;
    TDLibWrapper *wrapper = m_tdLibWrapper;
    descriptor.signalingDataEmitted = [wrapper, callId](const std::vector<uint8_t> &data) {
        if (!wrapper || data.empty()) {
            return;
        }
        QByteArray bytes(reinterpret_cast<const char *>(data.data()), static_cast<int>(data.size()));
        QMetaObject::invokeMethod(wrapper, "sendCallSignalingData", Qt::QueuedConnection,
                                  Q_ARG(qlonglong, callId), Q_ARG(QByteArray, bytes));
    };

    LOG("Creating tgcalls instance, version" << selectedVersion
        << "endpoints" << int(descriptor.endpoints.size())
        << "rtcServers" << int(descriptor.rtcServers.size()));
    m_instance = tgcalls::Meta::Create(selectedVersion.toStdString(), std::move(descriptor));
    if (!m_instance) {
        WARN("Failed to create tgcalls instance for version" << selectedVersion);
        return;
    }

    // Wire the video streams: start the camera + local preview, and route the
    // incoming (remote) video into the remote renderer.
    if (m_videoCapture) {
        m_videoCapture->setState(tgcalls::VideoState::Active);
        // Local self-view uses the same frame rotation as the remote (world-upright);
        // the rotating CallOverlay container makes both upright to the viewer.
        m_videoCapture->setOutput(m_localVideoRenderer->sink());
    }
    if (m_isVideo) {
        m_instance->setIncomingVideoOutput(m_remoteVideoRenderer->sink());
    }

    // Route the incoming-audio stream to the call sink and unmute it. The SFOS
    // pulse policy rule (pulse/harbour-fernschreiber.conf) puts the WebRTC streams
    // in the non-corking "call" group so the remote party is actually audible.
    m_audioRouter->start();

    while (!m_pendingSignalingData.isEmpty()) {
        m_instance->receiveSignalingData(toByteVector(m_pendingSignalingData.takeFirst()));
    }
}

void VoipManager::stopInstance()
{
    m_audioRouter->stop();
    m_pendingSignalingData.clear();
    if (m_videoCapture) {
        m_videoCapture->setState(tgcalls::VideoState::Inactive);
        m_videoCapture.reset();
    }
    m_remoteVideoRenderer->reset();
    m_localVideoRenderer->reset();
    if (!m_instance) {
        return;
    }
    // Tear down the tgcalls instance OFF the GUI thread: stop() is async but the
    // destructor joins the WebRTC threads and stops the PulseAudio device, which
    // blocks for a moment (or deadlocks) if done on the GUI thread — that froze
    // the UI on hang-up. Hand ownership to a detached thread so the UI stays live.
    std::shared_ptr<tgcalls::Instance> instance(m_instance.release());
    m_instance.reset();
    std::thread([instance]() mutable {
        instance->stop([](tgcalls::FinalState) {});
        instance.reset();
    }).detach();
}

void VoipManager::resetCall()
{
    m_currentCallId = 0;
    m_peerUserId = 0;
    m_isOutgoing = false;
    m_isVideo = false;
    m_frontCamera = true;
    if (m_remoteVideoActive) {
        m_remoteVideoActive = false;
        emit remoteVideoActiveChanged();
    }
    m_callState.clear();
    if (!m_emojis.isEmpty()) {
        m_emojis.clear();
        emit emojisChanged();
    }
    // callStateChanged drives `active` and `callState` bindings — without it the
    // call overlay (bound to voipManager.active) never hides after hang-up.
    emit callStateChanged();
    emit callChanged();
}

std::vector<uint8_t> VoipManager::toByteVector(const QByteArray &data)
{
    return std::vector<uint8_t>(
        reinterpret_cast<const uint8_t *>(data.constData()),
        reinterpret_cast<const uint8_t *>(data.constData()) + data.size());
}

QByteArray VoipManager::decodeTdlibBytes(const QString &data)
{
    if (data.isEmpty()) {
        return QByteArray();
    }
    const QByteArray decoded = QByteArray::fromBase64(data.toLatin1());
    return decoded.isEmpty() ? data.toUtf8() : decoded;
}
