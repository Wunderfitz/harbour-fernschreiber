/*
    Copyright (C) 2020-21 Sebastian J. Wolf and other contributors

    This file is part of Fernschreiber.

    Fernschreiber is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Fernschreiber is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Fernschreiber. If not, see <http://www.gnu.org/licenses/>.
*/

#include "fernschreiberutils.h"
#include <QMap>
#include <QVariant>
#include <QAudioEncoderSettings>
#include <QStandardPaths>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QUrl>
#include <QUrlQuery>
#include <QDateTime>
#include <QGeoCoordinate>
#include <QGeoLocation>
#include <QSysInfo>
#include <QNetworkRequest>
#include <QNetworkReply>

#define DEBUG_MODULE FernschreiberUtils
#include "debuglog.h"

namespace {
    const QString _TYPE("@type");
    const QString TEXT("text");
    const QString EMOJI("emoji");
    const QString ANIMATED_EMOJI("animated_emoji");
    const QString STICKER("sticker");
    const QString USER_ID("user_id");

    const QString MESSAGE_SENDER_TYPE_USER("messageSenderUser");

    const QString MESSAGE_CONTENT_TYPE_TEXT("messageText");
    const QString MESSAGE_CONTENT_TYPE_STICKER("messageSticker");
    const QString MESSAGE_CONTENT_TYPE_ANIMATED_EMOJI("messageAnimatedEmoji");
    const QString MESSAGE_CONTENT_TYPE_PHOTO("messagePhoto");
    const QString MESSAGE_CONTENT_TYPE_VIDEO("messageVideo");
    const QString MESSAGE_CONTENT_TYPE_VIDEO_NOTE("messageVideoNote");
    const QString MESSAGE_CONTENT_TYPE_ANIMATION("messageAnimation");
    const QString MESSAGE_CONTENT_TYPE_AUDIO("messageAudio");
    const QString MESSAGE_CONTENT_TYPE_VOICE_NOTE("messageVoiceNote");

    // Telegram renders a voice note as 100 bars of 5 bits each: as peaks are
    // collected at a much finer scale while recording and since the length
    // of the recording is only known once it is over, we resample to those
    // 100 bars only when the voice note is sent
    const int WAVEFORM_BAR_COUNT = 100;
    const int WAVEFORM_FRAMES_PER_PEAK = 1024;

    enum SampleReader {
        ReaderUnsupported,
        ReaderSignedInt8,
        ReaderUnsignedInt8,
        ReaderSignedInt16,
        ReaderUnsignedInt16,
        ReaderSignedInt32,
        ReaderFloat
    };

    SampleReader sampleReaderFor(const QAudioFormat &format)
    {
        switch (format.sampleType()) {
        case QAudioFormat::SignedInt:
            switch (format.sampleSize()) {
            case 8: return ReaderSignedInt8;
            case 16: return ReaderSignedInt16;
            case 32: return ReaderSignedInt32;
            }
            break;
        case QAudioFormat::UnSignedInt:
            switch (format.sampleSize()) {
            case 8: return ReaderUnsignedInt8;
            case 16: return ReaderUnsignedInt16;
            }
            break;
        case QAudioFormat::Float:
            if (format.sampleSize() == 32) {
                return ReaderFloat;
            }
            break;
        default:
            break;
        }
        return ReaderUnsupported;
    }

    // Amplitude of a single sample, scaled to fit regardless of the format
    // the capture pipeline happens to hand us
    quint16 sampleAmplitude(SampleReader reader, const char *sample)
    {
        switch (reader) {
        case ReaderSignedInt8:
            return quint16(qAbs(int(*reinterpret_cast<const qint8 *>(sample))) << 8);
        case ReaderUnsignedInt8:
            return quint16(qAbs(int(*reinterpret_cast<const quint8 *>(sample)) - 128) << 8);
        case ReaderSignedInt16:
            return quint16(qMin(qAbs(int(*reinterpret_cast<const qint16 *>(sample))), 0x7FFF));
        case ReaderUnsignedInt16:
            return quint16(qAbs(int(*reinterpret_cast<const quint16 *>(sample)) - 0x8000));
        case ReaderSignedInt32:
            return quint16(qAbs(qint64(*reinterpret_cast<const qint32 *>(sample))) >> 16);
        case ReaderFloat:
            return quint16(qBound(0.0f, qAbs(*reinterpret_cast<const float *>(sample)), 1.0f) * 0x7FFF);
        default:
            return 0;
        }
    }
    const QString MESSAGE_CONTENT_TYPE_DOCUMENT("messageDocument");
    const QString MESSAGE_CONTENT_TYPE_LOCATION("messageLocation");
    const QString MESSAGE_CONTENT_TYPE_LIVE_LOCATION("messageLiveLocation");
    const QString MESSAGE_CONTENT_TYPE_VENUE("messageVenue");
}

FernschreiberUtils::FernschreiberUtils(QObject *parent)
    : QObject(parent)
    , manager(new QNetworkAccessManager(this))
{
    LOG("Initializing audio recorder...");

    QString temporaryDirectoryPath = this->getTemporaryDirectoryPath();
    QDir temporaryDirectory(temporaryDirectoryPath);
    if (!temporaryDirectory.exists()) {
        temporaryDirectory.mkpath(temporaryDirectoryPath);
    }

    QAudioEncoderSettings encoderSettings;
    encoderSettings.setCodec("audio/vorbis");
    encoderSettings.setChannelCount(1);
    encoderSettings.setQuality(QMultimedia::LowQuality);
    this->audioRecorder.setEncodingSettings(encoderSettings);
    this->audioRecorder.setContainerFormat("ogg");

    this->voiceNoteDuration = 0;
    this->currentPeak = 0;
    this->currentPeakFrames = 0;
    this->sampleFormatReported = false;

    // Without the probe we simply send no waveform, which is what receivers
    // render as a flat bar... The voice note itself is unaffected
    if (this->audioProbe.setSource(&this->audioRecorder)) {
        connect(&audioProbe, SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(handleAudioBufferProbed(QAudioBuffer)));
    } else {
        LOG("Unable to monitor the recorded audio, voice notes will have no waveform");
    }

    QMediaRecorder::Status audioRecorderStatus = this->audioRecorder.status();
    this->handleAudioRecorderStatusChanged(audioRecorderStatus);

    connect(&audioRecorder, SIGNAL(durationChanged(qlonglong)), this, SLOT(handleVoiceNoteDurationChanged(qlonglong)));
    connect(&audioRecorder, SIGNAL(statusChanged(QMediaRecorder::Status)), this, SLOT(handleAudioRecorderStatusChanged(QMediaRecorder::Status)));

    this->geoPositionInfoSource = QGeoPositionInfoSource::createDefaultSource(this);
    if (this->geoPositionInfoSource) {
        LOG("Geolocation successfully initialized...");
        this->geoPositionInfoSource->setUpdateInterval(5000);
        connect(geoPositionInfoSource, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(handleGeoPositionUpdated(QGeoPositionInfo)));
    } else {
        LOG("Unable to initialize geolocation!");
    }
}

FernschreiberUtils::~FernschreiberUtils()
{
    this->cleanUp();
}

QString FernschreiberUtils::getMessageShortText(TDLibWrapper *tdLibWrapper, const QVariantMap &messageContent, const bool isChannel, const qlonglong currentUserId, const QVariantMap &messageSender)
{
    if (messageContent.isEmpty()) {
        return QString();
    }

    const QString contentType(messageContent.value(_TYPE).toString());
    const QString messageSenderType(messageSender.value(_TYPE).toString());
    const qlonglong messageSenderUserId = messageSender.value(USER_ID).toLongLong();
    const bool myself = !isChannel && (messageSenderType == MESSAGE_SENDER_TYPE_USER && messageSenderUserId == currentUserId);

    if (contentType == MESSAGE_CONTENT_TYPE_TEXT) {
        return messageContent.value(TEXT).toMap().value(TEXT).toString();
    }
    if (contentType == MESSAGE_CONTENT_TYPE_STICKER) {
        return messageContent.value(STICKER).toMap().value(EMOJI).toString();
    }
    if (contentType == MESSAGE_CONTENT_TYPE_ANIMATED_EMOJI) {
        return messageContent.value(ANIMATED_EMOJI).toMap().value(STICKER).toMap().value(EMOJI).toString();
    }
    if (contentType == MESSAGE_CONTENT_TYPE_PHOTO) {
        return myself ? tr("sent a picture", "myself") : tr("sent a picture");
    }
    if (contentType == MESSAGE_CONTENT_TYPE_VIDEO) {
        return myself ? tr("sent a video", "myself") : tr("sent a video");
    }
    if (contentType == MESSAGE_CONTENT_TYPE_VIDEO_NOTE) {
        return myself ? tr("sent a video note", "myself") : tr("sent a video note");
    }
    if (contentType == MESSAGE_CONTENT_TYPE_ANIMATION) {
        return myself ? tr("sent an animation", "myself") : tr("sent an animation");
    }
    if (contentType == MESSAGE_CONTENT_TYPE_AUDIO) {
        return myself ? tr("sent an audio", "myself") : tr("sent an audio");
    }
    if (contentType == MESSAGE_CONTENT_TYPE_VOICE_NOTE) {
        return myself ? tr("sent a voice note", "myself") : tr("sent a voice note");
    }
    if (contentType == MESSAGE_CONTENT_TYPE_DOCUMENT) {
        return myself ? tr("sent a document", "myself") : tr("sent a document");
    }
    if (contentType == MESSAGE_CONTENT_TYPE_LOCATION || contentType == MESSAGE_CONTENT_TYPE_LIVE_LOCATION) {
        return myself ? tr("sent a location", "myself") : tr("sent a location");
    }
    if (contentType == MESSAGE_CONTENT_TYPE_VENUE) {
        return myself ? tr("sent a venue", "myself") : tr("sent a venue");
    }
    if (contentType == "messageContact") {
        const QString contactName = getUserName(messageContent.value("contact").toMap());
        return myself ? tr("shared the contact %1", "myself; %1 is a name").arg(contactName) : tr("shared the contact %1", "%1 is a name").arg(contactName);
    }
    if (contentType == "messageContactRegistered") {
        return myself ? tr("have registered with Telegram", "myself") : tr("has registered with Telegram");
    }
    if (contentType == "messageChatJoinByLink") {
        return myself ? tr("joined this chat", "myself") : tr("joined this chat");
    }
    if (contentType == "messageChatAddMembers") {
        if (messageSenderType == MESSAGE_SENDER_TYPE_USER && messageSenderUserId == messageContent.value("member_user_ids").toList().at(0).toLongLong()) {
            return myself ? tr("were added to this chat", "myself") : tr("was added to this chat");
        } else {
            QVariantList memberUserIds = messageContent.value("member_user_ids").toList();
            QString addedUserNames;
            for (int i = 0; i < memberUserIds.size(); i++) {
                if (i > 0) {
                    addedUserNames += ", ";
                }
                addedUserNames += getUserName(tdLibWrapper->getUserInformation(memberUserIds.at(i).toString()));
            }
            return myself ? tr("have added %1 to the chat", "myself").arg(addedUserNames) : tr("has added %1 to the chat").arg(addedUserNames);
        }
    }
    if (contentType == "messageChatDeleteMember") {
        if (messageSenderType == MESSAGE_SENDER_TYPE_USER && messageSenderUserId == messageContent.value("user_id").toLongLong()) {
            return myself ? tr("left this chat", "myself") : tr("left this chat");
        } else {
            return myself ? tr("have removed %1 from the chat", "myself").arg(getUserName(tdLibWrapper->getUserInformation(messageContent.value("user_id").toString()))) : tr("has removed %1 from the chat").arg(getUserName(tdLibWrapper->getUserInformation(messageContent.value("user_id").toString())));
        }
    }
    if (contentType == "messageChatChangeTitle") {
        return myself ? tr("changed the chat title", "myself") : tr("changed the chat title");
    }
    if (contentType == "messagePoll") {
        if(messageContent.value("poll").toMap().value("type").toMap().value("@type").toString() == "pollTypeQuiz") {
            return myself ? tr("sent a quiz", "myself") : tr("sent a quiz");
        }
        return myself ? tr("sent a poll", "myself") : tr("sent a poll");
    }
    if (contentType == "messagePollOptionAdded") {
        const QString addedOption = messageContent.value(TEXT).toMap().value(TEXT).toString();
        return myself ? tr("have added the option %1 to a poll", "myself").arg(addedOption) : tr("has added the option %1 to a poll").arg(addedOption);
    }
    if (contentType == "messagePollOptionDeleted") {
        const QString deletedOption = messageContent.value(TEXT).toMap().value(TEXT).toString();
        return myself ? tr("have removed the option %1 from a poll", "myself").arg(deletedOption) : tr("has removed the option %1 from a poll").arg(deletedOption);
    }
    if (contentType == "messageBasicGroupChatCreate" || contentType == "messageSupergroupChatCreate") {
        return myself ? tr("created this group", "myself") : tr("created this group");
    }
    if (contentType == "messageChatChangePhoto") {
        return myself ? tr("changed the chat photo", "myself") : tr("changed the chat photo");
    }
    if (contentType == "messageChatDeletePhoto") {
        return myself ? tr("deleted the chat photo", "myself") : tr("deleted the chat photo");
    }
    if (contentType == "messageChatSetMessageAutoDeleteTime") {
        return myself ? tr("changed the secret chat TTL setting", "myself") : tr("changed the secret chat TTL setting");
    }
    if (contentType == "messageChatUpgradeFrom" || contentType == "messageChatUpgradeTo") {
        return myself ? tr("upgraded this group to a supergroup", "myself") : tr("upgraded this group to a supergroup");
    }
    if (contentType == "messageCustomServiceAction") {
        return messageContent.value(TEXT).toString();
    }
    if (contentType == "messagePinMessage") {
        return myself ? tr("changed the pinned message", "myself") : tr("changed the pinned message");
    }
    if (contentType == "messageExpiredPhoto") {
        return myself ? tr("sent a self-destructing photo that is expired", "myself") : tr("sent a self-destructing photo that is expired");
    }
    if (contentType == "messageExpiredVideo") {
        return myself ? tr("sent a self-destructing video that is expired", "myself") : tr("sent a self-destructing video that is expired");
    }
    if (contentType == "messageScreenshotTaken") {
        return myself ? tr("created a screenshot in this chat", "myself") : tr("created a screenshot in this chat");
    }
    if (contentType == "messageGameScore") {
        qint32 score = messageContent.value("score").toInt();
        return myself ? tr("scored %Ln points", "myself", score) : tr("scored %Ln points", "", score);
    }
    if (contentType == "messageGame") {
        return myself ? tr("sent a game", "myself") : tr("sent a game");
    }
    if (contentType == "messageUnsupported") {
        return myself ? tr("sent an unsupported message", "myself") : tr("sent an unsupported message");
    }

    return myself ? tr("sent an unsupported message: %1", "myself").arg(contentType.mid(7)) : tr("sent an unsupported message: %1").arg(contentType.mid(7));
}

QString FernschreiberUtils::getUserName(const QVariantMap &userInformation)
{
    const QString firstName = userInformation.value("first_name").toString();
    const QString lastName = userInformation.value("last_name").toString();
    return QString(firstName + " " + lastName).trimmed();
}

void FernschreiberUtils::startRecordingVoiceNote()
{
    LOG("Start recording voice note...");
    this->voiceNoteDuration = 0;
    this->voiceNotePeaks.clear();
    this->currentPeak = 0;
    this->currentPeakFrames = 0;
    QDateTime thisIsNow = QDateTime::currentDateTime();
    this->audioRecorder.setOutputLocation(QUrl::fromLocalFile(this->getTemporaryDirectoryPath() + "/voicenote-" + thisIsNow.toString("yyyy-MM-dd-HH-mm-ss") + ".ogg"));
    this->audioRecorder.setVolume(1);
    this->audioRecorder.record();
}

void FernschreiberUtils::stopRecordingVoiceNote()
{
    LOG("Stop recording voice note...");
    this->audioRecorder.stop();
}

QString FernschreiberUtils::voiceNotePath()
{
    return this->audioRecorder.outputLocation().toLocalFile();
}

qlonglong FernschreiberUtils::getVoiceNoteDuration()
{
    return this->voiceNoteDuration;
}

QString FernschreiberUtils::getVoiceNoteWaveform()
{
    QVector<quint16> peaks(this->voiceNotePeaks);
    if (this->currentPeakFrames > 0) {
        peaks.append(this->currentPeak);
    }
    if (peaks.isEmpty()) {
        return QString();
    }

    QVector<quint16> bars(WAVEFORM_BAR_COUNT, 0);
    quint16 loudest = 0;
    for (int i = 0; i < WAVEFORM_BAR_COUNT; i++) {
        const int from = (i * peaks.size()) / WAVEFORM_BAR_COUNT;
        const int to = qMin(qMax(from + 1, ((i + 1) * peaks.size()) / WAVEFORM_BAR_COUNT), peaks.size());
        for (int j = from; j < to; j++) {
            bars[i] = qMax(bars.at(i), peaks.at(j));
        }
        loudest = qMax(loudest, bars.at(i));
    }
    if (!loudest) {
        return QString();
    }

    // Five bits per bar, packed least significant bit first, looks like every
    // other Telegram client expects to read them back this way
    QByteArray waveform((WAVEFORM_BAR_COUNT * 5 + 7) / 8, char(0));
    for (int i = 0; i < WAVEFORM_BAR_COUNT; i++) {
        // Scaled against the loudest bar, so that a quietly recorded voice note
        // still gets a waveform instead of a barely visible line
        const int value = (int(bars.at(i)) * 31 + loudest / 2) / loudest;
        for (int bit = 0; bit < 5; bit++) {
            if (value & (1 << bit)) {
                const int offset = i * 5 + bit;
                waveform[offset / 8] = char(quint8(waveform.at(offset / 8)) | quint8(1 << (offset % 8)));
            }
        }
    }
    return QString::fromLatin1(waveform.toBase64());
}

FernschreiberUtils::VoiceNoteRecordingState FernschreiberUtils::getVoiceNoteRecordingState()
{
    return this->voiceNoteRecordingState;
}

void FernschreiberUtils::startGeoLocationUpdates()
{
    if (this->geoPositionInfoSource) {
        this->geoPositionInfoSource->startUpdates();
    }
}

void FernschreiberUtils::stopGeoLocationUpdates()
{
    if (this->geoPositionInfoSource) {
        this->geoPositionInfoSource->stopUpdates();
    }
}

bool FernschreiberUtils::supportsGeoLocation()
{
    return this->geoPositionInfoSource;
}

QString FernschreiberUtils::getSailfishOSVersion()
{
    return QSysInfo::productVersion();
}

void FernschreiberUtils::initiateReverseGeocode(double latitude, double longitude)
{
    LOG("Initiating reverse geocode:" << latitude << longitude);
    QUrl url = QUrl("https://nominatim.openstreetmap.org/reverse");
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("lat", QString::number(latitude));
    urlQuery.addQueryItem("lon", QString::number(longitude));
    urlQuery.addQueryItem("format", "json");
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Fernschreiber (Sailfish OS)");
    request.setRawHeader(QByteArray("Accept"), QByteArray("application/json"));
    request.setRawHeader(QByteArray("Accept-Charset"), QByteArray("utf-8"));
    request.setRawHeader(QByteArray("Connection"), QByteArray("close"));
    request.setRawHeader(QByteArray("Cache-Control"), QByteArray("max-age=0"));
    QNetworkReply *reply = manager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(handleReverseGeocodeFinished()));
}

void FernschreiberUtils::handleAudioBufferProbed(const QAudioBuffer &buffer)
{
    const QAudioFormat format = buffer.format();
    const SampleReader reader = sampleReaderFor(format);
    if (reader == ReaderUnsupported) {
        if (!this->sampleFormatReported) {
            LOG("Cannot build a waveform from sample type" << format.sampleType() << "of size" << format.sampleSize());
            this->sampleFormatReported = true;
        }
        return;
    }

    const int channelCount = qMax(1, format.channelCount());
    const int bytesPerSample = format.sampleSize() / 8;
    const int frameCount = buffer.frameCount();
    const char *data = static_cast<const char *>(buffer.constData());

    for (int frame = 0; frame < frameCount; frame++) {
        quint16 amplitude = 0;
        for (int channel = 0; channel < channelCount; channel++) {
            amplitude = qMax(amplitude, sampleAmplitude(reader, data + (frame * channelCount + channel) * bytesPerSample));
        }
        this->currentPeak = qMax(this->currentPeak, amplitude);
        if (++this->currentPeakFrames >= WAVEFORM_FRAMES_PER_PEAK) {
            this->voiceNotePeaks.append(this->currentPeak);
            this->currentPeak = 0;
            this->currentPeakFrames = 0;
        }
    }
}

void FernschreiberUtils::handleVoiceNoteDurationChanged(qlonglong duration)
{
    if (duration > this->voiceNoteDuration) {
        this->voiceNoteDuration = duration;
    }
    emit voiceNoteDurationChanged(this->voiceNoteDuration);
}

void FernschreiberUtils::handleAudioRecorderStatusChanged(QMediaRecorder::Status status)
{
    LOG("Audio recorder status changed:" << status);
    switch (status) {
    case QMediaRecorder::UnavailableStatus:
    case QMediaRecorder::UnloadedStatus:
    case QMediaRecorder::LoadingStatus:
        this->voiceNoteRecordingState = VoiceNoteRecordingState::Unavailable;
        break;
    case QMediaRecorder::LoadedStatus:
    case QMediaRecorder::PausedStatus:
        this->voiceNoteRecordingState = VoiceNoteRecordingState::Ready;
        break;
    case QMediaRecorder::StartingStatus:
        this->voiceNoteRecordingState = VoiceNoteRecordingState::Starting;
        break;
    case QMediaRecorder::FinalizingStatus:
        this->voiceNoteRecordingState = VoiceNoteRecordingState::Stopping;
        break;
    case QMediaRecorder::RecordingStatus:
        this->voiceNoteRecordingState = VoiceNoteRecordingState::Recording;
        break;
    }
    emit voiceNoteRecordingStateChanged(this->voiceNoteRecordingState);
}

void FernschreiberUtils::handleGeoPositionUpdated(const QGeoPositionInfo &info)
{
    LOG("Geo position was updated");
    QVariantMap positionInformation;
    if (info.hasAttribute(QGeoPositionInfo::HorizontalAccuracy)) {
        positionInformation.insert("horizontalAccuracy", info.attribute(QGeoPositionInfo::HorizontalAccuracy));
    } else {
        positionInformation.insert("horizontalAccuracy", 0);
    }
    if (info.hasAttribute(QGeoPositionInfo::VerticalAccuracy)) {
        positionInformation.insert("verticalAccuracy", info.attribute(QGeoPositionInfo::VerticalAccuracy));
    } else {
        positionInformation.insert("verticalAccuracy", 0);
    }
    QGeoCoordinate geoCoordinate = info.coordinate();
    positionInformation.insert("latitude", geoCoordinate.latitude());
    positionInformation.insert("longitude", geoCoordinate.longitude());

    this->initiateReverseGeocode(geoCoordinate.latitude(), geoCoordinate.longitude());

    emit newPositionInformation(positionInformation);
}

void FernschreiberUtils::handleReverseGeocodeFinished()
{
    qDebug() << "FernschreiberUtils::handleReverseGeocodeFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    qDebug().noquote() << jsonDocument.toJson(QJsonDocument::Indented);
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit newGeocodedAddress(responseObject.value("display_name").toString());
    }
}

void FernschreiberUtils::cleanUp()
{
    if (this->geoPositionInfoSource) {
        this->geoPositionInfoSource->stopUpdates();
    }
    QString temporaryDirectoryPath = this->getTemporaryDirectoryPath();
    QDirIterator temporaryDirectoryIterator(temporaryDirectoryPath, QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (temporaryDirectoryIterator.hasNext()) {
        QString nextFilePath = temporaryDirectoryIterator.next();
        if (QFile::remove(nextFilePath)) {
            LOG("Temporary file removed " << nextFilePath);
        } else {
            LOG("Error removing temporary file " << nextFilePath);
        }
    }
}

QString FernschreiberUtils::getTemporaryDirectoryPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::TempLocation) +  + "/harbour-fernschreiber";
}
