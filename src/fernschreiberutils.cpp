#include "fernschreiberutils.h"

#include <QMap>
#include <QVariant>

FernschreiberUtils::FernschreiberUtils(QObject *parent) : QObject(parent)
{

}

QString FernschreiberUtils::getMessageShortText(TDLibWrapper *tdLibWrapper, const QVariantMap &messageContent, const bool isChannel, const qlonglong currentUserId, const QVariantMap &messageSender)
{
    if (messageContent.isEmpty()) {
        return QString();
    }

    const bool myself = !isChannel && (messageSender.value("@type").toString() == "messageSenderUser" && messageSender.value("user_id").toLongLong() == currentUserId);

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
    if (contentType == "messageAudio") {
        return myself ? tr("sent an audio", "myself") : tr("sent an audio");
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
        if (messageSender.value("@type").toString() == "messageSenderUser" && messageSender.value("user_id").toLongLong() == messageContent.value("member_user_ids").toList().at(0).toLongLong()) {
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
        if (messageSender.value("@type").toString() == "messageSenderUser" && messageSender.value("user_id").toLongLong() == messageContent.value("user_id").toLongLong()) {
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

QString FernschreiberUtils::getUserName(const QVariantMap &userInformation)
{
    const QString firstName = userInformation.value("first_name").toString();
    const QString lastName = userInformation.value("last_name").toString();
    return QString(firstName + " " + lastName).trimmed();
}
