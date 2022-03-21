/*
    Copyright (C) 2020 Sebastian J. Wolf and other contributors

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

#include "tdlibwrapper.h"
#include "tdlibsecrets.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLocale>
#include <QProcess>
#include <QSysInfo>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

#define DEBUG_MODULE TDLibWrapper
#include "debuglog.h"

namespace {
    const QString STATUS("status");
    const QString ID("id");
    const QString CHAT_ID("chat_id");
    const QString MESSAGE_ID("message_id");
    const QString TYPE("type");
    const QString LAST_NAME("last_name");
    const QString FIRST_NAME("first_name");
    const QString USERNAME("username");
    const QString THREAD_ID("thread_id");
    const QString VALUE("value");
    const QString CHAT_LIST_TYPE("chat_list_type");
    const QString _TYPE("@type");
    const QString _EXTRA("@extra");
    const QString CHAT_LIST_MAIN("chatListMain");
}

TDLibWrapper::TDLibWrapper(AppSettings *appSettings, MceInterface *mceInterface, QObject *parent) : QObject(parent), joinChatRequested(false)
{
    LOG("Initializing TD Lib...");
    this->appSettings = appSettings;
    this->mceInterface = mceInterface;
    this->tdLibClient = td_json_client_create();
    this->authorizationState = AuthorizationState::Closed;
    this->isLoggingOut = false;

    initializeTDLibReciever();

    QString tdLibDatabaseDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/tdlib";
    QDir tdLibDatabaseDirectory(tdLibDatabaseDirectoryPath);
    if (!tdLibDatabaseDirectory.exists()) {
        tdLibDatabaseDirectory.mkpath(tdLibDatabaseDirectoryPath);
    }

    this->dbusInterface = new DBusInterface(this);
    if (this->appSettings->getUseOpenWith()) {
        this->initializeOpenWith();
    } else {
        this->removeOpenWith();
    }

    connect(&emojiSearchWorker, SIGNAL(searchCompleted(QString, QVariantList)), this, SLOT(handleEmojiSearchCompleted(QString, QVariantList)));

    connect(this->appSettings, SIGNAL(useOpenWithChanged()), this, SLOT(handleOpenWithChanged()));
    connect(this->appSettings, SIGNAL(storageOptimizerChanged()), this, SLOT(handleStorageOptimizerChanged()));

    this->setLogVerbosityLevel();
    this->setOptionInteger("notification_group_count_max", 5);
}

TDLibWrapper::~TDLibWrapper()
{
    LOG("Destroying TD Lib...");
    this->tdLibReceiver->setActive(false);
    while (this->tdLibReceiver->isRunning()) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
    }
    qDeleteAll(basicGroups.values());
    qDeleteAll(superGroups.values());
    td_json_client_destroy(this->tdLibClient);
}

void TDLibWrapper::initializeTDLibReciever() {
    this->tdLibReceiver = new TDLibReceiver(this->tdLibClient, this);
    connect(this->tdLibReceiver, SIGNAL(versionDetected(QString)), this, SLOT(handleVersionDetected(QString)));
    connect(this->tdLibReceiver, SIGNAL(authorizationStateChanged(QString, QVariantMap)), this, SLOT(handleAuthorizationStateChanged(QString, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(optionUpdated(QString, QVariant)), this, SLOT(handleOptionUpdated(QString, QVariant)));
    connect(this->tdLibReceiver, SIGNAL(connectionStateChanged(QString)), this, SLOT(handleConnectionStateChanged(QString)));
    connect(this->tdLibReceiver, SIGNAL(userUpdated(QVariantMap)), this, SLOT(handleUserUpdated(QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(userStatusUpdated(QString, QVariantMap)), this, SLOT(handleUserStatusUpdated(QString, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(fileUpdated(QVariantMap)), this, SLOT(handleFileUpdated(QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(newChatDiscovered(QVariantMap)), this, SLOT(handleNewChatDiscovered(QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(unreadMessageCountUpdated(QVariantMap)), this, SLOT(handleUnreadMessageCountUpdated(QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(unreadChatCountUpdated(QVariantMap)), this, SLOT(handleUnreadChatCountUpdated(QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(chatLastMessageUpdated(QString, QString, QVariantMap)), this, SIGNAL(chatLastMessageUpdated(QString, QString, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(chatOrderUpdated(QString, QString)), this, SIGNAL(chatOrderUpdated(QString, QString)));
    connect(this->tdLibReceiver, SIGNAL(chatReadInboxUpdated(QString, QString, int)), this, SIGNAL(chatReadInboxUpdated(QString, QString, int)));
    connect(this->tdLibReceiver, SIGNAL(chatReadOutboxUpdated(QString, QString)), this, SIGNAL(chatReadOutboxUpdated(QString, QString)));
    connect(this->tdLibReceiver, SIGNAL(basicGroupUpdated(qlonglong, QVariantMap)), this, SLOT(handleBasicGroupUpdated(qlonglong, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(superGroupUpdated(qlonglong, QVariantMap)), this, SLOT(handleSuperGroupUpdated(qlonglong, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(chatOnlineMemberCountUpdated(QString, int)), this, SIGNAL(chatOnlineMemberCountUpdated(QString, int)));
    connect(this->tdLibReceiver, SIGNAL(messagesReceived(QVariantList, int)), this, SIGNAL(messagesReceived(QVariantList, int)));
    connect(this->tdLibReceiver, SIGNAL(sponsoredMessageReceived(qlonglong, QVariantMap)), this, SLOT(handleSponsoredMessage(qlonglong, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(messageLinkInfoReceived(QString, QVariantMap, QString)), this, SIGNAL(messageLinkInfoReceived(QString, QVariantMap, QString)));
    connect(this->tdLibReceiver, SIGNAL(newMessageReceived(qlonglong, QVariantMap)), this, SIGNAL(newMessageReceived(qlonglong, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(messageInformation(qlonglong, qlonglong, QVariantMap)), this, SLOT(handleMessageInformation(qlonglong, qlonglong, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(messageSendSucceeded(qlonglong, qlonglong, QVariantMap)), this, SIGNAL(messageSendSucceeded(qlonglong, qlonglong, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(activeNotificationsUpdated(QVariantList)), this, SIGNAL(activeNotificationsUpdated(QVariantList)));
    connect(this->tdLibReceiver, SIGNAL(notificationGroupUpdated(QVariantMap)), this, SIGNAL(notificationGroupUpdated(QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(notificationUpdated(QVariantMap)), this, SIGNAL(notificationUpdated(QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(chatNotificationSettingsUpdated(QString, QVariantMap)), this, SIGNAL(chatNotificationSettingsUpdated(QString, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(messageContentUpdated(qlonglong, qlonglong, QVariantMap)), this, SIGNAL(messageContentUpdated(qlonglong, qlonglong, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(messagesDeleted(qlonglong, QList<qlonglong>)), this, SIGNAL(messagesDeleted(qlonglong, QList<qlonglong>)));
    connect(this->tdLibReceiver, SIGNAL(chats(QVariantMap)), this, SIGNAL(chatsReceived(QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(chat(QVariantMap)), this, SLOT(handleChatReceived(QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(secretChat(qlonglong, QVariantMap)), this, SLOT(handleSecretChatReceived(qlonglong, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(secretChatUpdated(qlonglong, QVariantMap)), this, SLOT(handleSecretChatUpdated(qlonglong, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(recentStickersUpdated(QVariantList)), this, SIGNAL(recentStickersUpdated(QVariantList)));
    connect(this->tdLibReceiver, SIGNAL(stickers(QVariantList)), this, SIGNAL(stickersReceived(QVariantList)));
    connect(this->tdLibReceiver, SIGNAL(installedStickerSetsUpdated(QVariantList)), this, SIGNAL(installedStickerSetsUpdated(QVariantList)));
    connect(this->tdLibReceiver, SIGNAL(stickerSets(QVariantList)), this, SLOT(handleStickerSets(QVariantList)));
    connect(this->tdLibReceiver, SIGNAL(stickerSet(QVariantMap)), this, SIGNAL(stickerSetReceived(QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(chatMembers(QString, QVariantList, int)), this, SIGNAL(chatMembersReceived(QString, QVariantList, int)));
    connect(this->tdLibReceiver, SIGNAL(userFullInfo(QVariantMap)), this, SIGNAL(userFullInfoReceived(QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(userFullInfoUpdated(QString, QVariantMap)), this, SIGNAL(userFullInfoUpdated(QString, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(basicGroupFullInfo(QString, QVariantMap)), this, SIGNAL(basicGroupFullInfoReceived(QString, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(basicGroupFullInfoUpdated(QString, QVariantMap)), this, SIGNAL(basicGroupFullInfoUpdated(QString, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(supergroupFullInfo(QString, QVariantMap)), this, SIGNAL(supergroupFullInfoReceived(QString, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(supergroupFullInfoUpdated(QString, QVariantMap)), this, SIGNAL(supergroupFullInfoUpdated(QString, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(userProfilePhotos(QString, QVariantList, int)), this, SIGNAL(userProfilePhotosReceived(QString, QVariantList, int)));
    connect(this->tdLibReceiver, SIGNAL(chatPermissionsUpdated(QString, QVariantMap)), this, SIGNAL(chatPermissionsUpdated(QString, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(chatPhotoUpdated(qlonglong, QVariantMap)), this, SIGNAL(chatPhotoUpdated(qlonglong, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(chatTitleUpdated(QString, QString)), this, SIGNAL(chatTitleUpdated(QString, QString)));
    connect(this->tdLibReceiver, SIGNAL(chatPinnedUpdated(qlonglong, bool)), this, SIGNAL(chatPinnedUpdated(qlonglong, bool)));
    connect(this->tdLibReceiver, SIGNAL(chatPinnedMessageUpdated(qlonglong, qlonglong)), this, SIGNAL(chatPinnedMessageUpdated(qlonglong, qlonglong)));
    connect(this->tdLibReceiver, SIGNAL(messageIsPinnedUpdated(qlonglong, qlonglong, bool)), this, SLOT(handleMessageIsPinnedUpdated(qlonglong, qlonglong, bool)));
    connect(this->tdLibReceiver, SIGNAL(usersReceived(QString, QVariantList, int)), this, SIGNAL(usersReceived(QString, QVariantList, int)));
    connect(this->tdLibReceiver, SIGNAL(errorReceived(int, QString, QString)), this, SLOT(handleErrorReceived(int, QString, QString)));
    connect(this->tdLibReceiver, SIGNAL(contactsImported(QVariantList, QVariantList)), this, SIGNAL(contactsImported(QVariantList, QVariantList)));
    connect(this->tdLibReceiver, SIGNAL(messageEditedUpdated(qlonglong, qlonglong, QVariantMap)), this, SIGNAL(messageEditedUpdated(qlonglong, qlonglong, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(chatIsMarkedAsUnreadUpdated(qlonglong, bool)), this, SIGNAL(chatIsMarkedAsUnreadUpdated(qlonglong, bool)));
    connect(this->tdLibReceiver, SIGNAL(chatDraftMessageUpdated(qlonglong, QVariantMap, QString)), this, SIGNAL(chatDraftMessageUpdated(qlonglong, QVariantMap, QString)));
    connect(this->tdLibReceiver, SIGNAL(inlineQueryResults(QString, QString, QVariantList, QString, QString, QString)), this, SIGNAL(inlineQueryResults(QString, QString, QVariantList, QString, QString, QString)));
    connect(this->tdLibReceiver, SIGNAL(callbackQueryAnswer(QString, bool, QString)), this, SIGNAL(callbackQueryAnswer(QString, bool, QString)));
    connect(this->tdLibReceiver, SIGNAL(userPrivacySettingRules(QVariantMap)), this, SLOT(handleUserPrivacySettingRules(QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(userPrivacySettingRulesUpdated(QVariantMap)), this, SLOT(handleUpdatedUserPrivacySettingRules(QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(messageInteractionInfoUpdated(qlonglong, qlonglong, QVariantMap)), this, SIGNAL(messageInteractionInfoUpdated(qlonglong, qlonglong, QVariantMap)));
    connect(this->tdLibReceiver, SIGNAL(okReceived(QString)), this, SIGNAL(okReceived(QString)));
    connect(this->tdLibReceiver, SIGNAL(sessionsReceived(QVariantList)), this, SIGNAL(sessionsReceived(QVariantList)));

    this->tdLibReceiver->start();
}

void TDLibWrapper::sendRequest(const QVariantMap &requestObject)
{
    if (this->isLoggingOut) {
        LOG("Sending request to TD Lib skipped as logging out is in progress, object type name:" << requestObject.value(_TYPE).toString());
        return;
    }
    LOG("Sending request to TD Lib, object type name:" << requestObject.value(_TYPE).toString());
    QJsonDocument requestDocument = QJsonDocument::fromVariant(requestObject);
    VERBOSE(requestDocument.toJson().constData());
    td_json_client_send(this->tdLibClient, requestDocument.toJson().constData());
}

QString TDLibWrapper::getVersion()
{
    return this->version;
}

TDLibWrapper::AuthorizationState TDLibWrapper::getAuthorizationState()
{
    return this->authorizationState;
}

QVariantMap TDLibWrapper::getAuthorizationStateData()
{
    return this->authorizationStateData;
}

TDLibWrapper::ConnectionState TDLibWrapper::getConnectionState()
{
    return this->connectionState;
}

void TDLibWrapper::setAuthenticationPhoneNumber(const QString &phoneNumber)
{
    LOG("Set authentication phone number " << phoneNumber);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "setAuthenticationPhoneNumber");
    requestObject.insert("phone_number", phoneNumber);
    QVariantMap phoneNumberSettings;
    phoneNumberSettings.insert("allow_flash_call", false);
    phoneNumberSettings.insert("is_current_phone_number", true);
    requestObject.insert("settings", phoneNumberSettings);
    this->sendRequest(requestObject);
}

void TDLibWrapper::setAuthenticationCode(const QString &authenticationCode)
{
    LOG("Set authentication code " << authenticationCode);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "checkAuthenticationCode");
    requestObject.insert("code", authenticationCode);
    this->sendRequest(requestObject);
}

void TDLibWrapper::setAuthenticationPassword(const QString &authenticationPassword)
{
    LOG("Set authentication password " << authenticationPassword);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "checkAuthenticationPassword");
    requestObject.insert("password", authenticationPassword);
    this->sendRequest(requestObject);
}

void TDLibWrapper::registerUser(const QString &firstName, const QString &lastName)
{
    LOG("Register User " << firstName << lastName);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "registerUser");
    requestObject.insert(FIRST_NAME, firstName);
    requestObject.insert(LAST_NAME, lastName);
    this->sendRequest(requestObject);
}

void TDLibWrapper::logout()
{
    LOG("Logging out");
    QVariantMap requestObject;
    requestObject.insert("@type", "logOut");
    this->sendRequest(requestObject);
    this->isLoggingOut = true;

}

void TDLibWrapper::getChats()
{
    LOG("Getting chats");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "loadChats");
    requestObject.insert("limit", 5);
    this->sendRequest(requestObject);
}

void TDLibWrapper::downloadFile(int fileId)
{
    LOG("Downloading file " << fileId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "downloadFile");
    requestObject.insert("file_id", fileId);
    requestObject.insert("synchronous", false);
    requestObject.insert("offset", 0);
    requestObject.insert("limit", 0);
    requestObject.insert("priority", 1);
    this->sendRequest(requestObject);
}

void TDLibWrapper::openChat(const QString &chatId)
{
    LOG("Opening chat " << chatId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "openChat");
    requestObject.insert(CHAT_ID, chatId);
    this->sendRequest(requestObject);
}

void TDLibWrapper::closeChat(const QString &chatId)
{
    LOG("Closing chat " << chatId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "closeChat");
    requestObject.insert(CHAT_ID, chatId);
    this->sendRequest(requestObject);
}

void TDLibWrapper::joinChat(const QString &chatId)
{
    LOG("Joining chat " << chatId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "joinChat");
    requestObject.insert(CHAT_ID, chatId);
    this->joinChatRequested = true;
    this->sendRequest(requestObject);
}

void TDLibWrapper::leaveChat(const QString &chatId)
{
    LOG("Leaving chat " << chatId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "leaveChat");
    requestObject.insert(CHAT_ID, chatId);
    this->sendRequest(requestObject);
}

void TDLibWrapper::deleteChat(qlonglong chatId)
{
    LOG("Deleting chat " << chatId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "deleteChat");
    requestObject.insert(CHAT_ID, chatId);
    this->sendRequest(requestObject);
}

void TDLibWrapper::getChatHistory(qlonglong chatId, qlonglong fromMessageId, int offset, int limit, bool onlyLocal)
{
    LOG("Retrieving chat history" << chatId << fromMessageId << offset << limit << onlyLocal);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getChatHistory");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert("from_message_id", fromMessageId);
    requestObject.insert("offset", offset);
    requestObject.insert("limit", limit);
    requestObject.insert("only_local", onlyLocal);
    this->sendRequest(requestObject);
}

void TDLibWrapper::viewMessage(const QString &chatId, const QString &messageId, bool force)
{
    LOG("Mark message as viewed" << chatId << messageId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "viewMessages");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert("force_read", force);
    QVariantList messageIds;
    messageIds.append(messageId);
    requestObject.insert("message_ids", messageIds);
    this->sendRequest(requestObject);
}

void TDLibWrapper::viewSponsoredMessage(qlonglong chatId, qlonglong messageId)
{
    LOG("Mark sponsored message as viewed" << chatId << messageId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "viewSponsoredMessage");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert("sponsored_message_id", messageId);
    requestObject.insert(_EXTRA, "viewSponsoredMessage");
    this->sendRequest(requestObject);
}

void TDLibWrapper::pinMessage(const QString &chatId, const QString &messageId, bool disableNotification)
{
    LOG("Pin message to chat" << chatId << messageId << disableNotification);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "pinChatMessage");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert(MESSAGE_ID, messageId);
    requestObject.insert("disable_notification", disableNotification);
    this->sendRequest(requestObject);
}

void TDLibWrapper::unpinMessage(const QString &chatId, const QString &messageId)
{
    LOG("Unpin message from chat" << chatId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "unpinChatMessage");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert(MESSAGE_ID, messageId);
    requestObject.insert(_EXTRA, "unpinChatMessage:" + chatId);
    this->sendRequest(requestObject);
}

static bool compareReplacements(const QVariant &replacement1, const QVariant &replacement2)
{
    const QVariantMap replacementMap1 = replacement1.toMap();
    const QVariantMap replacementMap2 = replacement2.toMap();

    if (replacementMap1.value("startIndex").toInt() < replacementMap2.value("startIndex").toInt()) {
        return true;
    } else {
        return false;
    }
}

void TDLibWrapper::sendTextMessage(const QString &chatId, const QString &message, const QString &replyToMessageId)
{
    LOG("Sending text message" << chatId << message << replyToMessageId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "sendMessage");
    requestObject.insert(CHAT_ID, chatId);
    if (replyToMessageId != "0") {
        requestObject.insert("reply_to_message_id", replyToMessageId);
    }
    QVariantMap inputMessageContent;
    inputMessageContent.insert(_TYPE, "inputMessageText");

    // Postprocess message (e.g. for @-mentioning)
    QString processedMessage = message;
    QVariantList replacements;
    QRegularExpression atMentionIdRegex("\\@(\\d+)\\(([^\\)]+)\\)");
    QRegularExpressionMatchIterator atMentionIdMatchIterator = atMentionIdRegex.globalMatch(processedMessage);
    while (atMentionIdMatchIterator.hasNext()) {
        QRegularExpressionMatch nextAtMentionId = atMentionIdMatchIterator.next();
        LOG("@Mentioning with user ID! Start Index: " << nextAtMentionId.capturedStart(0) << ", length: " << nextAtMentionId.capturedLength(0) << ", user ID: " << nextAtMentionId.captured(1) << ", plain text: " << nextAtMentionId.captured(2));
        QVariantMap replacement;
        replacement.insert("startIndex", nextAtMentionId.capturedStart(0));
        replacement.insert("length", nextAtMentionId.capturedLength(0));
        replacement.insert("userId", nextAtMentionId.captured(1));
        replacement.insert("plainText", nextAtMentionId.captured(2));
        replacements.append(replacement);
    }

    QVariantMap formattedText;

    if (!replacements.isEmpty()) {
        QVariantList entities;
        std::sort(replacements.begin(), replacements.end(), compareReplacements);
        QListIterator<QVariant> replacementsIterator(replacements);
        int offsetCorrection = 0;
        while (replacementsIterator.hasNext()) {
            QVariantMap nextReplacement = replacementsIterator.next().toMap();
            int replacementStartOffset = nextReplacement.value("startIndex").toInt();
            int replacementLength = nextReplacement.value("length").toInt();
            QString replacementPlainText = nextReplacement.value("plainText").toString();
            processedMessage = processedMessage.replace(replacementStartOffset - offsetCorrection, replacementLength, replacementPlainText);
            QVariantMap entity;
            entity.insert("offset", replacementStartOffset - offsetCorrection);
            entity.insert("length", replacementPlainText.length());
            QVariantMap entityType;
            entityType.insert(_TYPE, "textEntityTypeMentionName");
            entityType.insert("user_id", nextReplacement.value("userId").toString());
            entity.insert("type", entityType);
            entities.append(entity);
            offsetCorrection += replacementLength - replacementPlainText.length();
        }
        formattedText.insert("entities", entities);
    }

    formattedText.insert("text", processedMessage);
    formattedText.insert(_TYPE, "formattedText");
    inputMessageContent.insert("text", formattedText);
    requestObject.insert("input_message_content", inputMessageContent);
    this->sendRequest(requestObject);
}

void TDLibWrapper::sendPhotoMessage(const QString &chatId, const QString &filePath, const QString &message, const QString &replyToMessageId)
{
    LOG("Sending photo message" << chatId << filePath << message << replyToMessageId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "sendMessage");
    requestObject.insert(CHAT_ID, chatId);
    if (replyToMessageId != "0") {
        requestObject.insert("reply_to_message_id", replyToMessageId);
    }
    QVariantMap inputMessageContent;
    inputMessageContent.insert(_TYPE, "inputMessagePhoto");
    QVariantMap formattedText;
    formattedText.insert("text", message);
    formattedText.insert(_TYPE, "formattedText");
    inputMessageContent.insert("caption", formattedText);
    QVariantMap photoInputFile;
    photoInputFile.insert(_TYPE, "inputFileLocal");
    photoInputFile.insert("path", filePath);
    inputMessageContent.insert("photo", photoInputFile);

    requestObject.insert("input_message_content", inputMessageContent);
    this->sendRequest(requestObject);
}

void TDLibWrapper::sendVideoMessage(const QString &chatId, const QString &filePath, const QString &message, const QString &replyToMessageId)
{
    LOG("Sending video message" << chatId << filePath << message << replyToMessageId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "sendMessage");
    requestObject.insert(CHAT_ID, chatId);
    if (replyToMessageId != "0") {
        requestObject.insert("reply_to_message_id", replyToMessageId);
    }
    QVariantMap inputMessageContent;
    inputMessageContent.insert(_TYPE, "inputMessageVideo");
    QVariantMap formattedText;
    formattedText.insert("text", message);
    formattedText.insert(_TYPE, "formattedText");
    inputMessageContent.insert("caption", formattedText);
    QVariantMap videoInputFile;
    videoInputFile.insert(_TYPE, "inputFileLocal");
    videoInputFile.insert("path", filePath);
    inputMessageContent.insert("video", videoInputFile);

    requestObject.insert("input_message_content", inputMessageContent);
    this->sendRequest(requestObject);
}

void TDLibWrapper::sendDocumentMessage(const QString &chatId, const QString &filePath, const QString &message, const QString &replyToMessageId)
{
    LOG("Sending document message" << chatId << filePath << message << replyToMessageId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "sendMessage");
    requestObject.insert(CHAT_ID, chatId);
    if (replyToMessageId != "0") {
        requestObject.insert("reply_to_message_id", replyToMessageId);
    }
    QVariantMap inputMessageContent;
    inputMessageContent.insert(_TYPE, "inputMessageDocument");
    QVariantMap formattedText;
    formattedText.insert("text", message);
    formattedText.insert(_TYPE, "formattedText");
    inputMessageContent.insert("caption", formattedText);
    QVariantMap documentInputFile;
    documentInputFile.insert(_TYPE, "inputFileLocal");
    documentInputFile.insert("path", filePath);
    inputMessageContent.insert("document", documentInputFile);

    requestObject.insert("input_message_content", inputMessageContent);
    this->sendRequest(requestObject);
}

void TDLibWrapper::sendVoiceNoteMessage(const QString &chatId, const QString &filePath, const QString &message, const QString &replyToMessageId)
{
    LOG("Sending voice note message" << chatId << filePath << message << replyToMessageId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "sendMessage");
    requestObject.insert(CHAT_ID, chatId);
    if (replyToMessageId != "0") {
        requestObject.insert("reply_to_message_id", replyToMessageId);
    }
    QVariantMap inputMessageContent;
    inputMessageContent.insert(_TYPE, "inputMessageVoiceNote");
    QVariantMap formattedText;
    formattedText.insert("text", message);
    formattedText.insert(_TYPE, "formattedText");
    inputMessageContent.insert("caption", formattedText);
    QVariantMap documentInputFile;
    documentInputFile.insert(_TYPE, "inputFileLocal");
    documentInputFile.insert("path", filePath);
    inputMessageContent.insert("voice_note", documentInputFile);

    requestObject.insert("input_message_content", inputMessageContent);
    this->sendRequest(requestObject);
}

void TDLibWrapper::sendLocationMessage(const QString &chatId, double latitude, double longitude, double horizontalAccuracy, const QString &replyToMessageId)
{
    LOG("Sending location message" << chatId << latitude << longitude << horizontalAccuracy << replyToMessageId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "sendMessage");
    requestObject.insert(CHAT_ID, chatId);
    if (replyToMessageId != "0") {
        requestObject.insert("reply_to_message_id", replyToMessageId);
    }
    QVariantMap inputMessageContent;
    inputMessageContent.insert(_TYPE, "inputMessageLocation");
    QVariantMap location;
    location.insert("latitude", latitude);
    location.insert("longitude", longitude);
    location.insert("horizontal_accuracy", horizontalAccuracy);
    location.insert(_TYPE, "location");
    inputMessageContent.insert("location", location);

    inputMessageContent.insert("live_period", 0);
    inputMessageContent.insert("heading", 0);
    inputMessageContent.insert("proximity_alert_radius", 0);

    requestObject.insert("input_message_content", inputMessageContent);
    this->sendRequest(requestObject);
}

void TDLibWrapper::sendStickerMessage(const QString &chatId, const QString &fileId, const QString &replyToMessageId)
{
    LOG("Sending sticker message" << chatId << fileId << replyToMessageId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "sendMessage");
    requestObject.insert(CHAT_ID, chatId);
    if (replyToMessageId != "0") {
        requestObject.insert("reply_to_message_id", replyToMessageId);
    }
    QVariantMap inputMessageContent;
    inputMessageContent.insert(_TYPE, "inputMessageSticker");

    QVariantMap stickerInputFile;
    stickerInputFile.insert(_TYPE, "inputFileRemote");
    stickerInputFile.insert("id", fileId);

    inputMessageContent.insert("sticker", stickerInputFile);

    requestObject.insert("input_message_content", inputMessageContent);
    this->sendRequest(requestObject);
}

void TDLibWrapper::sendPollMessage(const QString &chatId, const QString &question, const QVariantList &options, bool anonymous, int correctOption, bool multiple, const QString &explanation, const QString &replyToMessageId)
{
    LOG("Sending poll message" << chatId << question << replyToMessageId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "sendMessage");
    requestObject.insert(CHAT_ID, chatId);
    if (replyToMessageId != "0") {
        requestObject.insert("reply_to_message_id", replyToMessageId);
    }
    QVariantMap inputMessageContent;
    inputMessageContent.insert(_TYPE, "inputMessagePoll");

    QVariantMap pollType;
    if(correctOption > -1) {
        pollType.insert(_TYPE, "pollTypeQuiz");
        pollType.insert("correct_option_id", correctOption);
        if(!explanation.isEmpty()) {
            QVariantMap formattedExplanation;
            formattedExplanation.insert("text", explanation);
            pollType.insert("explanation", formattedExplanation);
        }
    } else {
        pollType.insert(_TYPE, "pollTypeRegular");
        pollType.insert("allow_multiple_answers", multiple);
    }

    inputMessageContent.insert("type", pollType);
    inputMessageContent.insert("question", question);
    inputMessageContent.insert("options", options);
    inputMessageContent.insert("is_anonymous", anonymous);

    requestObject.insert("input_message_content", inputMessageContent);
    this->sendRequest(requestObject);
}

void TDLibWrapper::forwardMessages(const QString &chatId, const QString &fromChatId, const QVariantList &messageIds, bool sendCopy, bool removeCaption)
{
    LOG("Forwarding messages" << chatId << fromChatId << messageIds);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "forwardMessages");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert("from_chat_id", fromChatId);
    requestObject.insert("message_ids", messageIds);
    requestObject.insert("send_copy", sendCopy);
    requestObject.insert("remove_caption", removeCaption);

    this->sendRequest(requestObject);
}

void TDLibWrapper::getMessage(qlonglong chatId, qlonglong messageId)
{
    LOG("Retrieving message" << chatId << messageId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getMessage");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert(MESSAGE_ID, messageId);
    requestObject.insert(_EXTRA, QString("getMessage:%1:%2").arg(chatId).arg(messageId));
    this->sendRequest(requestObject);
}

void TDLibWrapper::getMessageLinkInfo(const QString &url, const QString &extra)
{
    LOG("Retrieving message link info" << url << extra);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getMessageLinkInfo");
    requestObject.insert("url", url);
    if (extra == "") {
        requestObject.insert(_EXTRA, url);
    } else {
        requestObject.insert(_EXTRA, url + "|" + extra);
    }

    this->sendRequest(requestObject);
}

void TDLibWrapper::getCallbackQueryAnswer(const QString &chatId, const QString &messageId, const QVariantMap &payload)
{
    LOG("Getting Callback Query Answer" << chatId << messageId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getCallbackQueryAnswer");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert(MESSAGE_ID, messageId);
    requestObject.insert("payload", payload);
    this->sendRequest(requestObject);
}

void TDLibWrapper::getChatPinnedMessage(qlonglong chatId)
{
    LOG("Retrieving pinned message" << chatId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getChatPinnedMessage");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert(_EXTRA, "getChatPinnedMessage:" + QString::number(chatId));
    this->sendRequest(requestObject);
}

void TDLibWrapper::getChatSponsoredMessage(qlonglong chatId)
{
    LOG("Retrieving sponsored message" << chatId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getChatSponsoredMessage");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert(_EXTRA, chatId); // see TDLibReceiver::processSponsoredMessage
    this->sendRequest(requestObject);
}

void TDLibWrapper::setOptionInteger(const QString &optionName, int optionValue)
{
    LOG("Setting integer option" << optionName << optionValue);
    setOption(optionName, "optionValueInteger", optionValue);
}

void TDLibWrapper::setOptionBoolean(const QString &optionName, bool optionValue)
{
    LOG("Setting boolean option" << optionName << optionValue);
    setOption(optionName, "optionValueBoolean", optionValue);
}

void TDLibWrapper::setOption(const QString &name, const QString &type, const QVariant &value)
{
    QVariantMap optionValue;
    optionValue.insert(_TYPE, type);
    optionValue.insert(VALUE, value);
    QVariantMap request;
    request.insert(_TYPE, "setOption");
    request.insert("name", name);
    request.insert(VALUE, optionValue);
    sendRequest(request);
}

void TDLibWrapper::setChatNotificationSettings(const QString &chatId, const QVariantMap &notificationSettings)
{
    LOG("Notification settings for chat " << chatId << notificationSettings);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "setChatNotificationSettings");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert("notification_settings", notificationSettings);
    this->sendRequest(requestObject);
}

void TDLibWrapper::editMessageText(const QString &chatId, const QString &messageId, const QString &message)
{
    LOG("Editing message text" << chatId << messageId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "editMessageText");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert(MESSAGE_ID, messageId);
    QVariantMap inputMessageContent;
    inputMessageContent.insert(_TYPE, "inputMessageText");
    QVariantMap formattedText;
    formattedText.insert("text", message);
    inputMessageContent.insert("text", formattedText);
    requestObject.insert("input_message_content", inputMessageContent);
    this->sendRequest(requestObject);
}

void TDLibWrapper::deleteMessages(const QString &chatId, const QVariantList messageIds)
{
    LOG("Deleting some messages" << chatId << messageIds);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "deleteMessages");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert("message_ids", messageIds);
    requestObject.insert("revoke", true);
    this->sendRequest(requestObject);
}

void TDLibWrapper::getMapThumbnailFile(const QString &chatId, double latitude, double longitude, int width, int height, const QString &extra)
{
    LOG("Getting Map Thumbnail File" << chatId);
    QVariantMap location;
    location.insert("latitude", latitude);
    location.insert("longitude", longitude);
    // ensure dimensions are in bounds (16 - 1024)
    int boundsWidth = std::min(std::max(width, 16), 1024);
    int boundsHeight = std::min(std::max(height, 16), 1024);

    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getMapThumbnailFile");
    requestObject.insert("location", location);
    requestObject.insert("zoom", 17); //13-20
    requestObject.insert("width", boundsWidth);
    requestObject.insert("height", boundsHeight);
    requestObject.insert("scale", 1); // 1-3
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert(_EXTRA, extra);

    this->sendRequest(requestObject);
}

void TDLibWrapper::getRecentStickers()
{
    LOG("Retrieving recent stickers");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getRecentStickers");
    this->sendRequest(requestObject);
}

void TDLibWrapper::getInstalledStickerSets()
{
    LOG("Retrieving installed sticker sets");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getInstalledStickerSets");
    this->sendRequest(requestObject);
}

void TDLibWrapper::getStickerSet(const QString &setId)
{
    LOG("Retrieving sticker set" << setId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getStickerSet");
    requestObject.insert("set_id", setId);
    this->sendRequest(requestObject);
}
void TDLibWrapper::getSupergroupMembers(const QString &groupId, int limit, int offset)
{
    LOG("Retrieving SupergroupMembers");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getSupergroupMembers");
    requestObject.insert(_EXTRA, groupId);
    requestObject.insert("supergroup_id", groupId);
    requestObject.insert("offset", offset);
    requestObject.insert("limit", limit);
    this->sendRequest(requestObject);
}

void TDLibWrapper::getGroupFullInfo(const QString &groupId, bool isSuperGroup)
{
    LOG("Retrieving GroupFullInfo");
    QVariantMap requestObject;
    if(isSuperGroup) {
        requestObject.insert(_TYPE, "getSupergroupFullInfo");
        requestObject.insert("supergroup_id", groupId);
    } else {
        requestObject.insert(_TYPE, "getBasicGroupFullInfo");
        requestObject.insert("basic_group_id", groupId);
    }
    requestObject.insert(_EXTRA, groupId);
    this->sendRequest(requestObject);
}

void TDLibWrapper::getUserFullInfo(const QString &userId)
{
    LOG("Retrieving UserFullInfo" << userId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getUserFullInfo");
    requestObject.insert(_EXTRA, userId);
    requestObject.insert("user_id", userId);
    this->sendRequest(requestObject);
}

void TDLibWrapper::createPrivateChat(const QString &userId, const QString &extra)
{
    LOG("Creating Private Chat");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "createPrivateChat");
    requestObject.insert("user_id", userId);
    requestObject.insert(_EXTRA, extra); //"openDirectly"/"openAndSendStartToBot:[optional parameter]" gets matched in qml
    this->sendRequest(requestObject);
}

void TDLibWrapper::createNewSecretChat(const QString &userId, const QString &extra)
{
    LOG("Creating new secret chat");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "createNewSecretChat");
    requestObject.insert("user_id", userId);
    requestObject.insert(_EXTRA, extra); //"openDirectly" gets matched in qml
    this->sendRequest(requestObject);
}

void TDLibWrapper::createSupergroupChat(const QString &supergroupId, const QString &extra)
{
    LOG("Creating Supergroup Chat");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "createSupergroupChat");
    requestObject.insert("supergroup_id", supergroupId);
    requestObject.insert(_EXTRA, extra); //"openDirectly" gets matched in qml
    this->sendRequest(requestObject);
}

void TDLibWrapper::createBasicGroupChat(const QString &basicGroupId, const QString &extra)
{
    LOG("Creating Basic Group Chat");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "createBasicGroupChat");
    requestObject.insert("basic_group_id", basicGroupId);
    requestObject.insert(_EXTRA, extra); //"openDirectly"/"openAndSend:*" gets matched in qml
    this->sendRequest(requestObject);
}

void TDLibWrapper::getGroupsInCommon(const QString &userId, int limit, int offset)
{
    LOG("Retrieving Groups in Common");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getGroupsInCommon");
    requestObject.insert(_EXTRA, userId);
    requestObject.insert("user_id", userId);
    requestObject.insert("offset", offset);
    requestObject.insert("limit", limit);
    this->sendRequest(requestObject);
}

void TDLibWrapper::getUserProfilePhotos(const QString &userId, int limit, int offset)
{
    LOG("Retrieving User Profile Photos");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getUserProfilePhotos");
    requestObject.insert(_EXTRA, userId);
    requestObject.insert("user_id", userId);
    requestObject.insert("offset", offset);
    requestObject.insert("limit", limit);
    this->sendRequest(requestObject);
}

void TDLibWrapper::setChatPermissions(const QString &chatId, const QVariantMap &chatPermissions)
{
    LOG("Setting Chat Permissions");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "setChatPermissions");
    requestObject.insert(_EXTRA, chatId);
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert("permissions", chatPermissions);
    this->sendRequest(requestObject);
}

void TDLibWrapper::setChatSlowModeDelay(const QString &chatId, int delay)
{

    LOG("Setting Chat Slow Mode Delay");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "setChatSlowModeDelay");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert("slow_mode_delay", delay);
    this->sendRequest(requestObject);
}

void TDLibWrapper::setChatDescription(const QString &chatId, const QString &description)
{
    LOG("Setting Chat Description");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "setChatDescription");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert("description", description);
    this->sendRequest(requestObject);
}

void TDLibWrapper::setChatTitle(const QString &chatId, const QString &title)
{
    LOG("Setting Chat Title");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "setChatTitle");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert("title", title);
    this->sendRequest(requestObject);
}

void TDLibWrapper::setBio(const QString &bio)
{
    LOG("Setting Bio");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "setBio");
    requestObject.insert("bio", bio);
    this->sendRequest(requestObject);
}

void TDLibWrapper::toggleSupergroupIsAllHistoryAvailable(const QString &groupId, bool isAllHistoryAvailable)
{
    LOG("Toggling SupergroupIsAllHistoryAvailable");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "toggleSupergroupIsAllHistoryAvailable");
    requestObject.insert("supergroup_id", groupId);
    requestObject.insert("is_all_history_available", isAllHistoryAvailable);
    this->sendRequest(requestObject);
}

void TDLibWrapper::setPollAnswer(const QString &chatId, qlonglong messageId, QVariantList optionIds)
{
    LOG("Setting Poll Answer");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "setPollAnswer");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert(MESSAGE_ID, messageId);
    requestObject.insert("option_ids", optionIds);
    this->sendRequest(requestObject);
}

void TDLibWrapper::stopPoll(const QString &chatId, qlonglong messageId)
{
    LOG("Stopping Poll");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "stopPoll");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert(MESSAGE_ID, messageId);
    this->sendRequest(requestObject);
}

void TDLibWrapper::getPollVoters(const QString &chatId, qlonglong messageId, int optionId, int limit, int offset, const QString &extra)
{
    LOG("Retrieving Poll Voters");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getPollVoters");
    requestObject.insert(_EXTRA, extra);
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert(MESSAGE_ID, messageId);
    requestObject.insert("option_id", optionId);
    requestObject.insert("offset", offset);
    requestObject.insert("limit", limit); //max 50
    this->sendRequest(requestObject);
}

void TDLibWrapper::searchPublicChat(const QString &userName, bool doOpenOnFound)
{
    LOG("Search public chat" << userName);
    if(doOpenOnFound) {
        this->activeChatSearchName = userName;
    }
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "searchPublicChat");
    requestObject.insert(_EXTRA, "searchPublicChat:"+userName);
    requestObject.insert(USERNAME, userName);
    this->sendRequest(requestObject);
}

void TDLibWrapper::joinChatByInviteLink(const QString &inviteLink)
{
    LOG("Join chat by invite link" << inviteLink);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "joinChatByInviteLink");
    requestObject.insert("invite_link", inviteLink);
    this->joinChatRequested = true;
    this->sendRequest(requestObject);
}

void TDLibWrapper::getDeepLinkInfo(const QString &link)
{
    LOG("Resolving TG deep link" << link);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getDeepLinkInfo");
    requestObject.insert("link", link);
    this->sendRequest(requestObject);
}

void TDLibWrapper::getContacts()
{
    LOG("Retrieving contacts");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getContacts");
    requestObject.insert(_EXTRA, "contactsRequested");
    this->sendRequest(requestObject);
}

void TDLibWrapper::getSecretChat(qlonglong secretChatId)
{
    LOG("Getting detailed information about secret chat" << secretChatId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getSecretChat");
    requestObject.insert("secret_chat_id", secretChatId);
    this->sendRequest(requestObject);
}

void TDLibWrapper::closeSecretChat(qlonglong secretChatId)
{
    LOG("Closing secret chat" << secretChatId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "closeSecretChat");
    requestObject.insert("secret_chat_id", secretChatId);
    this->sendRequest(requestObject);
}

void TDLibWrapper::importContacts(const QVariantList &contacts)
{
    LOG("Importing contacts");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "importContacts");
    requestObject.insert("contacts", contacts);
    this->sendRequest(requestObject);
}

void TDLibWrapper::searchChatMessages(qlonglong chatId, const QString &query, qlonglong fromMessageId)
{
    LOG("Searching for messages" << chatId << query << fromMessageId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "searchChatMessages");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert("query", query);
    requestObject.insert("from_message_id", fromMessageId);
    requestObject.insert("offset", 0);
    requestObject.insert("limit", 50);
    requestObject.insert(_EXTRA, "searchChatMessages");
    this->sendRequest(requestObject);
}

void TDLibWrapper::searchPublicChats(const QString &query)
{
    LOG("Searching public chats" << query);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "searchPublicChats");
    requestObject.insert("query", query);
    requestObject.insert(_EXTRA, "searchPublicChats");
    this->sendRequest(requestObject);
}

void TDLibWrapper::readAllChatMentions(qlonglong chatId)
{
    LOG("Read all chat mentions" << chatId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "readAllChatMentions");
    requestObject.insert(CHAT_ID, chatId);
    this->sendRequest(requestObject);
}

void TDLibWrapper::toggleChatIsMarkedAsUnread(qlonglong chatId, bool isMarkedAsUnread)
{
    LOG("Toggle chat is marked as unread" << chatId << isMarkedAsUnread);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "toggleChatIsMarkedAsUnread");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert("is_marked_as_unread", isMarkedAsUnread);
    this->sendRequest(requestObject);
}

void TDLibWrapper::toggleChatIsPinned(qlonglong chatId, bool isPinned)
{
    LOG("Toggle chat is pinned" << chatId << isPinned);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "toggleChatIsPinned");
    QVariantMap chatListMap;
    chatListMap.insert(_TYPE, CHAT_LIST_MAIN);
    requestObject.insert("chat_list", chatListMap);
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert("is_pinned", isPinned);
    requestObject.insert("is_marked_as_unread", isPinned);
    this->sendRequest(requestObject);
}

void TDLibWrapper::setChatDraftMessage(qlonglong chatId, qlonglong threadId, qlonglong replyToMessageId, const QString &draft)
{
    LOG("Set Draft Message" << chatId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "setChatDraftMessage");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert(THREAD_ID, threadId);
    QVariantMap draftMessage;
    QVariantMap inputMessageContent;
    QVariantMap formattedText;

    formattedText.insert("text", draft);
    formattedText.insert("clear_draft", draft.isEmpty());
    formattedText.insert(_TYPE, "formattedText");
    inputMessageContent.insert(_TYPE, "inputMessageText");
    inputMessageContent.insert("text", formattedText);
    draftMessage.insert(_TYPE, "draftMessage");
    draftMessage.insert("reply_to_message_id", replyToMessageId);
    draftMessage.insert("input_message_text", inputMessageContent);

    requestObject.insert("draft_message", draftMessage);
    this->sendRequest(requestObject);
}

void TDLibWrapper::getInlineQueryResults(qlonglong botUserId, qlonglong chatId, const QVariantMap &userLocation, const QString &query, const QString &offset, const QString &extra)
{

    LOG("Get Inline Query Results" << chatId << query);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getInlineQueryResults");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert("bot_user_id", botUserId);
    if(!userLocation.isEmpty()) {
        requestObject.insert("user_location", userLocation);
    }
    requestObject.insert("query", query);
    requestObject.insert("offset", offset);
    requestObject.insert(_EXTRA, extra);

    this->sendRequest(requestObject);
}

void TDLibWrapper::sendInlineQueryResultMessage(qlonglong chatId, qlonglong threadId, qlonglong replyToMessageId, const QString &queryId, const QString &resultId)
{

    LOG("Send Inline Query Result Message" << chatId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "sendInlineQueryResultMessage");
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert("message_thread_id", threadId);
    requestObject.insert("reply_to_message_id", replyToMessageId);
    requestObject.insert("query_id", queryId);
    requestObject.insert("result_id", resultId);

    this->sendRequest(requestObject);
}

void TDLibWrapper::sendBotStartMessage(qlonglong botUserId, qlonglong chatId, const QString &parameter, const QString &extra)
{

    LOG("Send Bot Start Message" << botUserId << chatId << parameter << extra);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "sendBotStartMessage");
    requestObject.insert("bot_user_id", botUserId);
    requestObject.insert(CHAT_ID, chatId);
    requestObject.insert("parameter", parameter);
    requestObject.insert(_EXTRA, extra);

    this->sendRequest(requestObject);
}

void TDLibWrapper::cancelDownloadFile(int fileId)
{
    LOG("Cancel Download File" << fileId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "cancelDownloadFile");
    requestObject.insert("file_id", fileId);
    requestObject.insert("only_if_pending", false);

    this->sendRequest(requestObject);
}

void TDLibWrapper::cancelUploadFile(int fileId)
{
    LOG("Cancel Upload File" << fileId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "cancelUploadFile");
    requestObject.insert("file_id", fileId);

    this->sendRequest(requestObject);
}

void TDLibWrapper::deleteFile(int fileId)
{
    LOG("Delete cached File" << fileId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "deleteFile");
    requestObject.insert("file_id", fileId);

    this->sendRequest(requestObject);
}

void TDLibWrapper::setName(const QString &firstName, const QString &lastName)
{
    LOG("Set name of current user" << firstName << lastName);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "setName");
    requestObject.insert("first_name", firstName);
    requestObject.insert("last_name", lastName);

    this->sendRequest(requestObject);
}

void TDLibWrapper::setUsername(const QString &userName)
{
    LOG("Set username of current user" << userName);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "setUsername");
    requestObject.insert("username", userName);

    this->sendRequest(requestObject);
}

void TDLibWrapper::setUserPrivacySettingRule(TDLibWrapper::UserPrivacySetting setting, TDLibWrapper::UserPrivacySettingRule rule)
{
    LOG("Set user privacy setting rule of current user" << setting << rule);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "setUserPrivacySettingRules");

    QVariantMap settingMap;
    switch (setting) {
    case SettingShowStatus:
        settingMap.insert(_TYPE, "userPrivacySettingShowStatus");
        break;
    case SettingShowPhoneNumber:
        settingMap.insert(_TYPE, "userPrivacySettingShowPhoneNumber");
        break;
    case SettingAllowChatInvites:
        settingMap.insert(_TYPE, "userPrivacySettingAllowChatInvites");
        break;
    case SettingShowProfilePhoto:
        settingMap.insert(_TYPE, "userPrivacySettingShowProfilePhoto");
        break;
    case SettingAllowFindingByPhoneNumber:
        settingMap.insert(_TYPE, "userPrivacySettingAllowFindingByPhoneNumber");
        break;
    case SettingShowLinkInForwardedMessages:
        settingMap.insert(_TYPE, "userPrivacySettingShowLinkInForwardedMessages");
        break;
    case SettingUnknown:
        return;
    }
    requestObject.insert("setting", settingMap);


    QVariantMap ruleMap;
    switch (rule) {
    case RuleAllowAll:
        ruleMap.insert(_TYPE, "userPrivacySettingRuleAllowAll");
        break;
    case RuleAllowContacts:
        ruleMap.insert(_TYPE, "userPrivacySettingRuleAllowContacts");
        break;
    case RuleRestrictAll:
        ruleMap.insert(_TYPE, "userPrivacySettingRuleRestrictAll");
        break;
    }
    QVariantList ruleMaps;
    ruleMaps.append(ruleMap);
    QVariantMap encapsulatedRules;
    encapsulatedRules.insert(_TYPE, "userPrivacySettingRules");
    encapsulatedRules.insert("rules", ruleMaps);
    requestObject.insert("rules", encapsulatedRules);

    this->sendRequest(requestObject);
}

void TDLibWrapper::getUserPrivacySettingRules(TDLibWrapper::UserPrivacySetting setting)
{
    LOG("Getting user privacy setting rules of current user" << setting);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getUserPrivacySettingRules");
    requestObject.insert(_EXTRA, setting);

    QVariantMap settingMap;
    switch (setting) {
    case SettingShowStatus:
        settingMap.insert(_TYPE, "userPrivacySettingShowStatus");
        break;
    case SettingShowPhoneNumber:
        settingMap.insert(_TYPE, "userPrivacySettingShowPhoneNumber");
        break;
    case SettingAllowChatInvites:
        settingMap.insert(_TYPE, "userPrivacySettingAllowChatInvites");
        break;
    case SettingShowProfilePhoto:
        settingMap.insert(_TYPE, "userPrivacySettingShowProfilePhoto");
        break;
    case SettingAllowFindingByPhoneNumber:
        settingMap.insert(_TYPE, "userPrivacySettingAllowFindingByPhoneNumber");
        break;
    case SettingShowLinkInForwardedMessages:
        settingMap.insert(_TYPE, "userPrivacySettingShowLinkInForwardedMessages");
        break;
    case SettingUnknown:
        return;
    }
    requestObject.insert("setting", settingMap);

    this->sendRequest(requestObject);
}

void TDLibWrapper::setProfilePhoto(const QString &filePath)
{
    LOG("Set a profile photo" << filePath);

    QVariantMap requestObject;
    requestObject.insert(_TYPE, "setProfilePhoto");
    requestObject.insert(_EXTRA, "setProfilePhoto");
    QVariantMap inputChatPhoto;
    inputChatPhoto.insert(_TYPE, "inputChatPhotoStatic");
    QVariantMap inputFile;
    inputFile.insert(_TYPE, "inputFileLocal");
    inputFile.insert("path", filePath);
    inputChatPhoto.insert("photo", inputFile);
    requestObject.insert("photo", inputChatPhoto);

    this->sendRequest(requestObject);
}

void TDLibWrapper::deleteProfilePhoto(const QString &profilePhotoId)
{
    LOG("Delete a profile photo" << profilePhotoId);

    QVariantMap requestObject;
    requestObject.insert(_TYPE, "deleteProfilePhoto");
    requestObject.insert(_EXTRA, "deleteProfilePhoto");
    requestObject.insert("profile_photo_id", profilePhotoId);

    this->sendRequest(requestObject);
}

void TDLibWrapper::changeStickerSet(const QString &stickerSetId, bool isInstalled)
{
    LOG("Change sticker set" << stickerSetId << isInstalled);

    QVariantMap requestObject;
    requestObject.insert(_TYPE, "changeStickerSet");
    requestObject.insert(_EXTRA, isInstalled ? "installStickerSet" : "removeStickerSet");
    requestObject.insert("set_id", stickerSetId);
    requestObject.insert("is_installed", isInstalled);

    this->sendRequest(requestObject);
}

void TDLibWrapper::getActiveSessions()
{
    LOG("Get active sessions");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "getActiveSessions");
    this->sendRequest(requestObject);
}

void TDLibWrapper::terminateSession(const QString &sessionId)
{
    LOG("Terminate session" << sessionId);
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "terminateSession");
    requestObject.insert(_EXTRA, "terminateSession");
    requestObject.insert("session_id", sessionId);
    this->sendRequest(requestObject);
}

void TDLibWrapper::searchEmoji(const QString &queryString)
{
    LOG("Searching emoji" << queryString);
    while (this->emojiSearchWorker.isRunning()) {
        this->emojiSearchWorker.requestInterruption();
    }
    this->emojiSearchWorker.setParameters(queryString);
    this->emojiSearchWorker.start();
}

QVariantMap TDLibWrapper::getUserInformation()
{
    return this->userInformation;
}

QVariantMap TDLibWrapper::getUserInformation(const QString &userId)
{
    // LOG("Returning user information for ID" << userId);
    return this->allUsers.value(userId).toMap();
}

bool TDLibWrapper::hasUserInformation(const QString &userId)
{
    return this->allUsers.contains(userId);
}

QVariantMap TDLibWrapper::getUserInformationByName(const QString &userName)
{
    return this->allUserNames.value(userName).toMap();
}

TDLibWrapper::UserPrivacySettingRule TDLibWrapper::getUserPrivacySettingRule(TDLibWrapper::UserPrivacySetting userPrivacySetting)
{
    return this->userPrivacySettingRules.value(userPrivacySetting, UserPrivacySettingRule::RuleAllowAll);
}

QVariantMap TDLibWrapper::getUnreadMessageInformation()
{
    return this->unreadMessageInformation;
}

QVariantMap TDLibWrapper::getUnreadChatInformation()
{
    return this->unreadChatInformation;
}

QVariantMap TDLibWrapper::getBasicGroup(qlonglong groupId) const
{
    const Group* group = basicGroups.value(groupId);
    if (group) {
        LOG("Returning basic group information for ID" << groupId);
        return group->groupInfo;
    } else {
        LOG("No super group information for ID" << groupId);
        return QVariantMap();
    }
}

QVariantMap TDLibWrapper::getSuperGroup(qlonglong groupId) const
{
    const Group* group = superGroups.value(groupId);
    if (group) {
        LOG("Returning super group information for ID" << groupId);
        return group->groupInfo;
    } else {
        LOG("No super group information for ID" << groupId);
        return QVariantMap();
    }
}

QVariantMap TDLibWrapper::getChat(const QString &chatId)
{
    LOG("Returning chat information for ID" << chatId);
    return this->chats.value(chatId).toMap();
}

QVariantMap TDLibWrapper::getSecretChatFromCache(qlonglong secretChatId)
{
    return this->secretChats.value(secretChatId);
}

QString TDLibWrapper::getOptionString(const QString &optionName)
{
    return this->options.value(optionName).toString();
}

void TDLibWrapper::copyFileToDownloads(const QString &filePath, bool openAfterCopy)
{
    LOG("Copy file to downloads" << filePath << openAfterCopy);
    QFileInfo fileInfo(filePath);
    if (fileInfo.exists()) {
        QString downloadFilePath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/" + fileInfo.fileName();
        if (QFile::exists(downloadFilePath)) {
            if (openAfterCopy) {
                this->openFileOnDevice(downloadFilePath);
            } else {
                emit copyToDownloadsSuccessful(fileInfo.fileName(), downloadFilePath);
            }
        } else {
            if (QFile::copy(filePath, downloadFilePath)) {
                if (openAfterCopy) {
                    this->openFileOnDevice(downloadFilePath);
                } else {
                    emit copyToDownloadsSuccessful(fileInfo.fileName(), downloadFilePath);
                }
            } else {
                emit copyToDownloadsError(fileInfo.fileName(), downloadFilePath);
            }
        }
    } else {
        emit copyToDownloadsError(fileInfo.fileName(), filePath);
    }
}

void TDLibWrapper::openFileOnDevice(const QString &filePath)
{
    LOG("Open file on device:" << filePath);
    QStringList argumentsList;
    argumentsList.append(filePath);
    bool successfullyStarted = QProcess::startDetached("xdg-open", argumentsList);
    if (successfullyStarted) {
        LOG("Successfully opened file " << filePath);
    } else {
        LOG("Error opening file " << filePath);
    }
}

void TDLibWrapper::controlScreenSaver(bool enabled)
{
    if (enabled) {
        mceInterface->displayCancelBlankingPause();
    } else {
        mceInterface->displayBlankingPause();
    }
}

bool TDLibWrapper::getJoinChatRequested()
{
    return this->joinChatRequested;
}

void TDLibWrapper::registerJoinChat()
{
    this->joinChatRequested = false;
}

DBusAdaptor *TDLibWrapper::getDBusAdaptor()
{
    return this->dbusInterface->getDBusAdaptor();
}

void TDLibWrapper::handleVersionDetected(const QString &version)
{
    this->version = version;
    emit versionDetected(version);
}

void TDLibWrapper::handleAuthorizationStateChanged(const QString &authorizationState, const QVariantMap authorizationStateData)
{
    if (authorizationState == "authorizationStateClosed") {
        this->authorizationState = AuthorizationState::Closed;
    }

    if (authorizationState == "authorizationStateClosing") {
        this->authorizationState = AuthorizationState::Closing;
    }

    if (authorizationState == "authorizationStateLoggingOut") {
        this->authorizationState = AuthorizationState::LoggingOut;
    }

    if (authorizationState == "authorizationStateReady") {
        this->authorizationState = AuthorizationState::AuthorizationReady;
    }

    if (authorizationState == "authorizationStateWaitCode") {
        this->authorizationState = AuthorizationState::WaitCode;
    }

    if (authorizationState == "authorizationStateWaitEncryptionKey") {
        this->setEncryptionKey();
        this->authorizationState = AuthorizationState::WaitEncryptionKey;
    }

    if (authorizationState == "authorizationStateWaitOtherDeviceConfirmation") {
        this->authorizationState = AuthorizationState::WaitOtherDeviceConfirmation;
    }

    if (authorizationState == "authorizationStateWaitPassword") {
        this->authorizationState = AuthorizationState::WaitPassword;
    }

    if (authorizationState == "authorizationStateWaitPhoneNumber") {
        this->authorizationState = AuthorizationState::WaitPhoneNumber;
    }

    if (authorizationState == "authorizationStateWaitRegistration") {
        this->authorizationState = AuthorizationState::WaitRegistration;
    }

    if (authorizationState == "authorizationStateWaitTdlibParameters") {
        this->setInitialParameters();
        this->authorizationState = AuthorizationState::WaitTdlibParameters;
    }
    if (authorizationState == "authorizationStateLoggingOut") {
        this->authorizationState = AuthorizationState::AuthorizationStateLoggingOut;
    }
    if (authorizationState == "authorizationStateClosed") {
        this->authorizationState = AuthorizationState::AuthorizationStateClosed;
        LOG("Reloading TD Lib...");
        this->basicGroups.clear();
        this->superGroups.clear();
        this->allUsers.clear();
        this->allUserNames.clear();
        this->tdLibReceiver->setActive(false);
        while (this->tdLibReceiver->isRunning()) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
        }
        td_json_client_destroy(this->tdLibClient);
        this->tdLibReceiver->terminate();
        QDir appPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
        appPath.removeRecursively();
        this->tdLibClient = td_json_client_create();
        initializeTDLibReciever();
        this->isLoggingOut = false;
    }
    this->authorizationStateData = authorizationStateData;
    emit authorizationStateChanged(this->authorizationState, this->authorizationStateData);

}

void TDLibWrapper::handleOptionUpdated(const QString &optionName, const QVariant &optionValue)
{
    this->options.insert(optionName, optionValue);
    emit optionUpdated(optionName, optionValue);
    if (optionName == "my_id") {
        emit ownUserIdFound(optionValue.toString());
    }
}

void TDLibWrapper::handleConnectionStateChanged(const QString &connectionState)
{
    if (connectionState == "connectionStateConnecting") {
        this->connectionState = ConnectionState::Connecting;
    }
    if (connectionState == "connectionStateConnectingToProxy") {
        this->connectionState = ConnectionState::ConnectingToProxy;
    }
    if (connectionState == "connectionStateReady") {
        this->connectionState = ConnectionState::ConnectionReady;
    }
    if (connectionState == "connectionStateUpdating") {
        this->connectionState = ConnectionState::Updating;
    }
    if (connectionState == "connectionStateWaitingForNetwork") {
        this->connectionState = ConnectionState::WaitingForNetwork;
    }

    emit connectionStateChanged(this->connectionState);
}

void TDLibWrapper::handleUserUpdated(const QVariantMap &userInformation)
{
    QString updatedUserId = userInformation.value("id").toString();
    if (updatedUserId == this->options.value("my_id").toString()) {
        LOG("Own user information updated :)");
        this->userInformation = userInformation;
        emit ownUserUpdated(userInformation);
    }
    LOG("User information updated:" << userInformation.value(USERNAME).toString() << userInformation.value(FIRST_NAME).toString() << userInformation.value(LAST_NAME).toString());
    this->allUsers.insert(updatedUserId, userInformation);
    this->allUserNames.insert(userInformation.value(USERNAME).toString(), userInformation);
    emit userUpdated(updatedUserId, userInformation);
}

void TDLibWrapper::handleUserStatusUpdated(const QString &userId, const QVariantMap &userStatusInformation)
{
    if (userId == this->options.value("my_id").toString()) {
        LOG("Own user status information updated :)");
        this->userInformation.insert("status", userStatusInformation);
    }
    LOG("User status information updated:" << userId << userStatusInformation.value(_TYPE).toString());
    QVariantMap updatedUserInformation = this->allUsers.value(userId).toMap();
    updatedUserInformation.insert("status", userStatusInformation);
    this->allUsers.insert(userId, updatedUserInformation);
    this->allUserNames.insert(userInformation.value(USERNAME).toString(), userInformation);
    emit userUpdated(userId, updatedUserInformation);
}

void TDLibWrapper::handleFileUpdated(const QVariantMap &fileInformation)
{
    emit fileUpdated(fileInformation.value("id").toInt(), fileInformation);
}

void TDLibWrapper::handleNewChatDiscovered(const QVariantMap &chatInformation)
{
    QString chatId = chatInformation.value("id").toString();
    this->chats.insert(chatId, chatInformation);
    emit newChatDiscovered(chatId, chatInformation);
}

void TDLibWrapper::handleChatReceived(const QVariantMap &chatInformation)
{
    emit chatReceived(chatInformation);
    if (!this->activeChatSearchName.isEmpty()) {
        QVariantMap chatType = chatInformation.value(TYPE).toMap();
        ChatType receivedChatType = chatTypeFromString(chatType.value(_TYPE).toString());
        if (receivedChatType == ChatTypeBasicGroup) {
            LOG("Found basic group for active search" << this->activeChatSearchName);
            this->activeChatSearchName.clear();
            this->createBasicGroupChat(chatType.value("basic_group_id").toString(), "openDirectly");
        }
        if (receivedChatType == ChatTypeSupergroup) {
            LOG("Found supergroup for active search" << this->activeChatSearchName);
            this->activeChatSearchName.clear();
            this->createSupergroupChat(chatType.value("supergroup_id").toString(), "openDirectly");
        }
    }
}

void TDLibWrapper::handleUnreadMessageCountUpdated(const QVariantMap &messageCountInformation)
{
    if (messageCountInformation.value(CHAT_LIST_TYPE).toString() == CHAT_LIST_MAIN) {
        this->unreadMessageInformation = messageCountInformation;
        emit unreadMessageCountUpdated(messageCountInformation);
    }
}

void TDLibWrapper::handleUnreadChatCountUpdated(const QVariantMap &chatCountInformation)
{
    if (chatCountInformation.value(CHAT_LIST_TYPE).toString() == CHAT_LIST_MAIN) {
        this->unreadChatInformation = chatCountInformation;
        emit unreadChatCountUpdated(chatCountInformation);
    }
}

void TDLibWrapper::handleBasicGroupUpdated(qlonglong groupId, const QVariantMap &groupInformation)
{
    emit basicGroupUpdated(updateGroup(groupId, groupInformation, &basicGroups)->groupId);
    if (!this->activeChatSearchName.isEmpty() && this->activeChatSearchName == groupInformation.value(USERNAME).toString()) {
        LOG("Found basic group for active search" << this->activeChatSearchName);
        this->activeChatSearchName.clear();
        this->createBasicGroupChat(groupInformation.value(ID).toString(), "openDirectly");
    }
}

void TDLibWrapper::handleSuperGroupUpdated(qlonglong groupId, const QVariantMap &groupInformation)
{
    emit superGroupUpdated(updateGroup(groupId, groupInformation, &superGroups)->groupId);
    if (!this->activeChatSearchName.isEmpty() && this->activeChatSearchName == groupInformation.value(USERNAME).toString()) {
        LOG("Found supergroup for active search" << this->activeChatSearchName);
        this->activeChatSearchName.clear();
        this->createSupergroupChat(groupInformation.value(ID).toString(), "openDirectly");
    }
}

void TDLibWrapper::handleStickerSets(const QVariantList &stickerSets)
{
    QListIterator<QVariant> stickerSetIterator(stickerSets);
    while (stickerSetIterator.hasNext()) {
        QVariantMap stickerSet = stickerSetIterator.next().toMap();
        this->getStickerSet(stickerSet.value("id").toString());
    }
    emit this->stickerSetsReceived(stickerSets);
}

void TDLibWrapper::handleEmojiSearchCompleted(const QString &queryString, const QVariantList &resultList)
{
    LOG("Emoji search completed" << queryString);
    emit emojiSearchSuccessful(resultList);
}

void TDLibWrapper::handleOpenWithChanged()
{
    if (this->appSettings->getUseOpenWith()) {
        this->initializeOpenWith();
    } else {
        this->removeOpenWith();
    }
}

void TDLibWrapper::handleSecretChatReceived(qlonglong secretChatId, const QVariantMap &secretChat)
{
    this->secretChats.insert(secretChatId, secretChat);
    emit secretChatReceived(secretChatId, secretChat);
}

void TDLibWrapper::handleSecretChatUpdated(qlonglong secretChatId, const QVariantMap &secretChat)
{
    this->secretChats.insert(secretChatId, secretChat);
    emit secretChatUpdated(secretChatId, secretChat);
}

void TDLibWrapper::handleStorageOptimizerChanged()
{
    setOptionBoolean("use_storage_optimizer", appSettings->storageOptimizer());
}

void TDLibWrapper::handleErrorReceived(int code, const QString &message, const QString &extra)
{
    if (!extra.isEmpty()) {
        QStringList parts(extra.split(':'));
        if (parts.size() == 3 && parts.at(0) == QStringLiteral("getMessage")) {
            emit messageNotFound(parts.at(1).toLongLong(), parts.at(2).toLongLong());
        }
    }
    emit errorReceived(code, message, extra);
}

void TDLibWrapper::handleMessageInformation(qlonglong chatId, qlonglong messageId, const QVariantMap &receivedInformation)
{
    QString extraInformation = receivedInformation.value(_EXTRA).toString();
    if (extraInformation.startsWith("getChatPinnedMessage:")) {
        emit chatPinnedMessageUpdated(chatId, messageId);
    }
    emit receivedMessage(chatId, messageId, receivedInformation);
}

void TDLibWrapper::handleMessageIsPinnedUpdated(qlonglong chatId, qlonglong messageId, bool isPinned)
{
    if (isPinned) {
        emit chatPinnedMessageUpdated(chatId, messageId);
    } else {
        emit chatPinnedMessageUpdated(chatId, 0);
        this->getChatPinnedMessage(chatId);
    }
}

void TDLibWrapper::handleUserPrivacySettingRules(const QVariantMap &rules)
{
    QVariantList newGivenRules = rules.value("rules").toList();
    // If nothing (or something unsupported is sent out) it is considered to be restricted completely
    UserPrivacySettingRule newAppliedRule = UserPrivacySettingRule::RuleRestrictAll;
    QListIterator<QVariant> givenRulesIterator(newGivenRules);
    while (givenRulesIterator.hasNext()) {
        QString givenRule = givenRulesIterator.next().toMap().value(_TYPE).toString();
        if (givenRule == "userPrivacySettingRuleAllowContacts") {
            newAppliedRule = UserPrivacySettingRule::RuleAllowContacts;
        }
        if (givenRule == "userPrivacySettingRuleAllowAll") {
            newAppliedRule = UserPrivacySettingRule::RuleAllowAll;
        }
    }
    UserPrivacySetting usedSetting = static_cast<UserPrivacySetting>(rules.value(_EXTRA).toInt());
    this->userPrivacySettingRules.insert(usedSetting, newAppliedRule);
    emit userPrivacySettingUpdated(usedSetting, newAppliedRule);
}

void TDLibWrapper::handleUpdatedUserPrivacySettingRules(const QVariantMap &updatedRules)
{
    QString rawSetting = updatedRules.value("setting").toMap().value(_TYPE).toString();
    UserPrivacySetting usedSetting = UserPrivacySetting::SettingUnknown;
    if (rawSetting == "userPrivacySettingAllowChatInvites") {
        usedSetting = UserPrivacySetting::SettingAllowChatInvites;
    }
    if (rawSetting == "userPrivacySettingAllowFindingByPhoneNumber") {
        usedSetting = UserPrivacySetting::SettingAllowFindingByPhoneNumber;
    }
    if (rawSetting == "userPrivacySettingShowLinkInForwardedMessages") {
        usedSetting = UserPrivacySetting::SettingShowLinkInForwardedMessages;
    }
    if (rawSetting == "userPrivacySettingShowPhoneNumber") {
        usedSetting = UserPrivacySetting::SettingShowPhoneNumber;
    }
    if (rawSetting == "userPrivacySettingShowProfilePhoto") {
        usedSetting = UserPrivacySetting::SettingShowProfilePhoto;
    }
    if (rawSetting == "userPrivacySettingShowStatus") {
        usedSetting = UserPrivacySetting::SettingShowStatus;
    }
    if (usedSetting != UserPrivacySetting::SettingUnknown) {
        QVariantMap rawRules = updatedRules.value("rules").toMap();
        rawRules.insert(_EXTRA, usedSetting);
        this->handleUserPrivacySettingRules(rawRules);
    }
}

void TDLibWrapper::handleSponsoredMessage(qlonglong chatId, const QVariantMap &message)
{
    switch (appSettings->getSponsoredMess()) {
    case AppSettings::SponsoredMessHandle:
        emit sponsoredMessageReceived(chatId, message);
        break;
    case AppSettings::SponsoredMessAutoView:
        LOG("Auto-viewing sponsored message");
        viewSponsoredMessage(chatId, message.value(ID).toULongLong());
        break;
    case AppSettings::SponsoredMessIgnore:
        LOG("Ignoring sponsored message");
        break;
    }
}

void TDLibWrapper::setInitialParameters()
{
    LOG("Sending initial parameters to TD Lib");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "setTdlibParameters");
    QVariantMap initialParameters;
    initialParameters.insert("api_id", TDLIB_API_ID);
    initialParameters.insert("api_hash", TDLIB_API_HASH);
    initialParameters.insert("database_directory", QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/tdlib");
    bool onlineOnlyMode = this->appSettings->onlineOnlyMode();
    initialParameters.insert("use_file_database", !onlineOnlyMode);
    initialParameters.insert("use_chat_info_database", !onlineOnlyMode);
    initialParameters.insert("use_message_database", !onlineOnlyMode);
    initialParameters.insert("use_secret_chats", true);
    initialParameters.insert("system_language_code", QLocale::system().name());
    QSettings hardwareSettings("/etc/hw-release", QSettings::NativeFormat);
    initialParameters.insert("device_model", hardwareSettings.value("NAME", "Unknown Mobile Device").toString());
    initialParameters.insert("system_version", QSysInfo::prettyProductName());
    initialParameters.insert("application_version", "0.14");
    initialParameters.insert("enable_storage_optimizer", appSettings->storageOptimizer());
    // initialParameters.insert("use_test_dc", true);
    requestObject.insert("parameters", initialParameters);
    this->sendRequest(requestObject);
}

void TDLibWrapper::setEncryptionKey()
{
    LOG("Setting database encryption key");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "checkDatabaseEncryptionKey");
    // see https://github.com/tdlib/td/issues/188#issuecomment-379536139
    requestObject.insert("encryption_key", "");
    this->sendRequest(requestObject);
}

void TDLibWrapper::setLogVerbosityLevel()
{
    LOG("Setting log verbosity level to something less chatty");
    QVariantMap requestObject;
    requestObject.insert(_TYPE, "setLogVerbosityLevel");
    requestObject.insert("new_verbosity_level", 2);
    this->sendRequest(requestObject);
}

void TDLibWrapper::initializeOpenWith()
{
    LOG("Initialize open-with");LOG("Checking standard open URL file...");

    const QStringList sailfishOSVersion = QSysInfo::productVersion().split(".");
    int sailfishOSMajorVersion = sailfishOSVersion.value(0).toInt();
    int sailfishOSMinorVersion = sailfishOSVersion.value(1).toInt();

    const QString applicationsLocation(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation));
    const QString openUrlFilePath(applicationsLocation + "/open-url.desktop");
    if (sailfishOSMajorVersion < 4 || ( sailfishOSMajorVersion == 4 && sailfishOSMinorVersion < 2 )) {
        if (QFile::exists(openUrlFilePath)) {
            LOG("Standard open URL file exists, good!");
        } else {
            LOG("Copying standard open URL file to " << openUrlFilePath);
            QFile::copy("/usr/share/applications/open-url.desktop", openUrlFilePath);
            QProcess::startDetached("update-desktop-database " + applicationsLocation);
        }
    } else {
        const QString sailfishBrowserFilePath(applicationsLocation + "/sailfish-browser.desktop");
        if (QFile::exists(sailfishBrowserFilePath)) {
            LOG("Removing existing local Sailfish browser file, that was not working as expected in 0.10...!");
            QFile::remove(sailfishBrowserFilePath);
            QProcess::startDetached("update-desktop-database " + applicationsLocation);
        }
        if (QFile::exists(openUrlFilePath)) {
            LOG("Old open URL file exists, that needs to go away...!");
            QFile::remove(openUrlFilePath);
            QProcess::startDetached("update-desktop-database " + applicationsLocation);
        }
        // Something special for Verla...
        if (sailfishOSMajorVersion == 4 && sailfishOSMinorVersion == 2) {
            LOG("Creating open URL file at " << openUrlFilePath);
            QFile openUrlFile(openUrlFilePath);
            if (openUrlFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream fileOut(&openUrlFile);
                fileOut.setCodec("UTF-8");
                fileOut << QString("[Desktop Entry]").toUtf8() << "\n";
                fileOut << QString("Type=Application").toUtf8() << "\n";
                fileOut << QString("Name=Browser").toUtf8() << "\n";
                fileOut << QString("Icon=icon-launcher-browser").toUtf8() << "\n";
                fileOut << QString("NoDisplay=true").toUtf8() << "\n";
                fileOut << QString("X-MeeGo-Logical-Id=sailfish-browser-ap-name").toUtf8() << "\n";
                fileOut << QString("X-MeeGo-Translation-Catalog=sailfish-browser").toUtf8() << "\n";
                fileOut << QString("MimeType=text/html;x-scheme-handler/http;x-scheme-handler/https;").toUtf8() << "\n";
                fileOut << QString("X-Maemo-Service=org.sailfishos.browser.ui").toUtf8() << "\n";
                fileOut << QString("X-Maemo-Object-Path=/ui").toUtf8() << "\n";
                fileOut << QString("X-Maemo-Method=org.sailfishos.browser.ui.openUrl").toUtf8() << "\n";
                fileOut.flush();
                openUrlFile.close();
                QProcess::startDetached("update-desktop-database " + applicationsLocation);
            }
        }
    }

    const QString desktopFilePath(applicationsLocation + "/harbour-fernschreiber-open-url.desktop");
    QFile desktopFile(desktopFilePath);
    if (desktopFile.exists()) {
        LOG("Fernschreiber open-with file existing, removing...");
        desktopFile.remove();
        QProcess::startDetached("update-desktop-database " + applicationsLocation);
    }
    LOG("Creating Fernschreiber open-with file at " << desktopFile.fileName());
    if (desktopFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream fileOut(&desktopFile);
        fileOut.setCodec("UTF-8");
        fileOut << QString("[Desktop Entry]").toUtf8() << "\n";
        fileOut << QString("Type=Application").toUtf8() << "\n";
        fileOut << QString("Name=Fernschreiber").toUtf8() << "\n";
        fileOut << QString("Icon=harbour-fernschreiber").toUtf8() << "\n";
        fileOut << QString("NotShowIn=X-MeeGo;").toUtf8() << "\n";
        if (sailfishOSMajorVersion < 4 || ( sailfishOSMajorVersion == 4 && sailfishOSMinorVersion < 1 )) {
            fileOut << QString("MimeType=text/html;x-scheme-handler/http;x-scheme-handler/https;x-scheme-handler/tg;").toUtf8() << "\n";
        } else {
            fileOut << QString("MimeType=x-url-handler/t.me;x-scheme-handler/tg;").toUtf8() << "\n";
        }
        fileOut << QString("X-Maemo-Service=de.ygriega.fernschreiber").toUtf8() << "\n";
        fileOut << QString("X-Maemo-Object-Path=/de/ygriega/fernschreiber").toUtf8() << "\n";
        fileOut << QString("X-Maemo-Method=de.ygriega.fernschreiber.openUrl").toUtf8() << "\n";
        fileOut << QString("Hidden=true;").toUtf8() << "\n";
        fileOut.flush();
        desktopFile.close();
        QProcess::startDetached("update-desktop-database " + applicationsLocation);
    }

    QString dbusPathName = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/dbus-1/services";
    QDir dbusPath(dbusPathName);
    if (!dbusPath.exists()) {
        LOG("Creating D-Bus directory" << dbusPathName);
        dbusPath.mkpath(dbusPathName);
    }
    QString dbusServiceFileName = dbusPathName + "/de.ygriega.fernschreiber.service";
    QFile dbusServiceFile(dbusServiceFileName);
    if (!dbusServiceFile.exists()) {
        LOG("Creating D-Bus service file at" << dbusServiceFile.fileName());
        if (dbusServiceFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream fileOut(&dbusServiceFile);
            fileOut.setCodec("UTF-8");
            fileOut << QString("[D-BUS Service]").toUtf8() << "\n";
            fileOut << QString("Name=de.ygriega.fernschreiber").toUtf8() << "\n";
            fileOut << QString("Exec=/usr/bin/invoker -s --type=silica-qt5 /usr/bin/harbour-fernschreiber").toUtf8() << "\n";
            fileOut.flush();
            dbusServiceFile.close();
        }
    }
}

void TDLibWrapper::removeOpenWith()
{
    LOG("Remove open-with");
    QFile::remove(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + "/harbour-fernschreiber-open-url.desktop");
    QProcess::startDetached("update-desktop-database " + QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation));
}

const TDLibWrapper::Group *TDLibWrapper::updateGroup(qlonglong groupId, const QVariantMap &groupInfo, QHash<qlonglong,Group*> *groups)
{
    Group* group = groups->value(groupId);
    if (!group) {
        group = new Group(groupId);
        groups->insert(groupId, group);
    }
    group->groupInfo = groupInfo;
    return group;
}

const TDLibWrapper::Group* TDLibWrapper::getGroup(qlonglong groupId) const
{
    if (groupId) {
        const Group* group = superGroups.value(groupId);
        return group ? group : basicGroups.value(groupId);
    }
    return Q_NULLPTR;
}

TDLibWrapper::ChatType TDLibWrapper::chatTypeFromString(const QString &type)
{
    return (type == QStringLiteral("chatTypePrivate")) ? ChatTypePrivate :
        (type == QStringLiteral("chatTypeBasicGroup")) ? ChatTypeBasicGroup :
        (type == QStringLiteral("chatTypeSupergroup")) ? ChatTypeSupergroup :
        (type == QStringLiteral("chatTypeSecret")) ?  ChatTypeSecret :
        ChatTypeUnknown;
}

TDLibWrapper::ChatMemberStatus TDLibWrapper::chatMemberStatusFromString(const QString &status)
{
    // Most common ones first
    return (status == QStringLiteral("chatMemberStatusMember")) ? ChatMemberStatusMember :
        (status == QStringLiteral("chatMemberStatusLeft")) ? ChatMemberStatusLeft :
        (status == QStringLiteral("chatMemberStatusCreator")) ? ChatMemberStatusCreator :
        (status == QStringLiteral("chatMemberStatusAdministrator")) ?  ChatMemberStatusAdministrator :
        (status == QStringLiteral("chatMemberStatusRestricted")) ? ChatMemberStatusRestricted :
        (status == QStringLiteral("chatMemberStatusBanned")) ?  ChatMemberStatusBanned :
                                                                ChatMemberStatusUnknown;
}

TDLibWrapper::SecretChatState TDLibWrapper::secretChatStateFromString(const QString &state)
{
    return (state == QStringLiteral("secretChatStateClosed")) ? SecretChatStateClosed :
        (state == QStringLiteral("secretChatStatePending")) ? SecretChatStatePending :
        (state == QStringLiteral("secretChatStateReady")) ? SecretChatStateReady :
        SecretChatStateUnknown;
}

TDLibWrapper::ChatMemberStatus TDLibWrapper::Group::chatMemberStatus() const
{
    const QString statusType(groupInfo.value(STATUS).toMap().value(_TYPE).toString());
    return statusType.isEmpty() ? ChatMemberStatusUnknown : chatMemberStatusFromString(statusType);
}
