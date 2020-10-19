#include "fernschreiberutils.h"

#include <QMap>
#include <QVariant>

FernschreiberUtils::FernschreiberUtils(QObject *parent) : QObject(parent)
{

}

QString FernschreiberUtils::getMessageShortText(const QVariantMap &messageContent, const bool &myself)
{
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
    return tr("Unsupported message: %1").arg(contentType.mid(7));
}
