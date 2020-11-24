#include "fernschreiberutils.h"

#include <QMap>
#include <QVariant>

FernschreiberUtils::FernschreiberUtils(QObject *parent) : QObject(parent)
{

}

QString FernschreiberUtils::getMessageShortText(const QVariantMap &messageContent, const bool &myself)
{
    if (messageContent.isEmpty()) {
        return QString();
    }

    QString contentType = messageContent.value("@type").toString();

    if (contentType == "messageText") {
        return messageContent.value("text").toMap().value("text").toString();
    }
    if (contentType == "messageSticker") {
        return tr("Sticker: %1").arg(messageContent.value("sticker").toMap().value("emoji").toString());
    }
    if (contentType == "messagePhoto") {
        return myself ? tr("sent a picture", "myself") : tr("sent a picture");
    }
    if (contentType == "messageVideo") {
        return myself ? tr("sent a video", "myself") : tr("sent a video");
    }
    if (contentType == "messageAnimation") {
        return myself ? tr("sent an animation", "myself") : tr("sent an animation");
    }
    if (contentType == "messageVoiceNote") {
        return myself ? tr("sent a voice note", "myself") : tr("sent a voice note");
    }
    if (contentType == "messageDocument") {
        return myself ? tr("sent a document", "myself") : tr("sent a document");
    }
    if (contentType == "messageLocation") {
        return myself ? tr("sent a location", "myself") : tr("sent a location");
    }
    if (contentType == "messageVenue") {
        return myself ? tr("sent a venue", "myself") : tr("sent a venue");
    }
    if (contentType == "messageContactRegistered") {
        return myself ? tr("have registered with Telegram", "myself") : tr("has registered with Telegram");
    }
    if (contentType == "messageChatJoinByLink") {
        return myself ? tr("joined this chat", "myself") : tr("joined this chat");
    }
    if (contentType == "messageChatAddMembers") {
        return myself ? tr("were added to this chat", "myself") : tr("was added to this chat");
    }
    if (contentType == "messageChatDeleteMember") {
        return myself ? tr("left this chat", "myself") : tr("left this chat");
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
    if (contentType == "messageBasicGroupChatCreate" || contentType == "messageSupergroupChatCreate") {
        return myself ? tr("created this group", "myself") : tr("created this group");
    }
    if (contentType == "messageChatChangePhoto") {
        return myself ? tr("changed the chat photo", "myself") : tr("changed the chat photo");
    }
    if (contentType == "messageChatDeletePhoto") {
        return myself ? tr("deleted the chat photo", "myself") : tr("deleted the chat photo");
    }
    if (contentType == "messageChatSetTtl") {
        return myself ? tr("changed the secret chat TTL setting", "myself") : tr("changed the secret chat TTL setting");
    }
    if (contentType == "messageChatUpgradeFrom" || contentType == "messageChatUpgradeTo") {
        return myself ? tr("upgraded this group to a supergroup", "myself") : tr("upgraded this group to a supergroup");
    }
    if (contentType == "messageCustomServiceAction") {
        return messageContent.value("text").toString();
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
    if (contentType == "messageUnsupported") {
        return myself ? tr("sent an unsupported message", "myself") : tr("sent an unsupported message");
    }

    return myself ? tr("sent an unsupported message: %1", "myself").arg(contentType.mid(7)) : tr("sent an unsupported message: %1").arg(contentType.mid(7));
}
