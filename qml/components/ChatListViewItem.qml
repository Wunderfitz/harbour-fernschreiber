import QtQuick 2.6
import Sailfish.Silica 1.0
import WerkWolf.Fernschreiber 1.0

import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions

PhotoTextsListItem {
    id: listItem
    pictureThumbnail {
        photoData: photo_small || ({})
        highlighted: listItem.highlighted && !listItem.menuOpen
    }
    property int ownUserId
    property bool showDraft: !!draft_message_text && draft_message_date > last_message_date
    property string previewText: showDraft ? draft_message_text : last_message_text

    // chat title
    primaryText.text: title ? Emoji.emojify(title, Theme.fontSizeMedium) : qsTr("Unknown")
    // last user
    prologSecondaryText.text: showDraft ? "<i>"+qsTr("Draft")+"</i>" : (is_channel ? "" : ( last_message_sender_id ? ( last_message_sender_id !== ownUserId ? Emoji.emojify(Functions.getUserName(tdLibWrapper.getUserInformation(last_message_sender_id)), Theme.fontSizeExtraSmall) : qsTr("You") ) : "" ))
    // last message
    secondaryText.text: previewText ? Emoji.emojify(Functions.enhanceHtmlEntities(previewText), Theme.fontSizeExtraSmall) : "<i>" + qsTr("No message in this chat.") + "</i>"
    // message date
    tertiaryText.text: showDraft ? Functions.getDateTimeElapsed(draft_message_date) : ( last_message_date ? ( last_message_date.length === 0 ? "" : Functions.getDateTimeElapsed(last_message_date) + Emoji.emojify(last_message_status, tertiaryText.font.pixelSize) ) : "" )
    unreadCount: unread_count
    unreadReactionCount: unread_reaction_count
    unreadMentionCount: unread_mention_count
    isSecret: ( chat_type === TelegramAPI.ChatTypeSecret )
    isMarkedAsUnread: is_marked_as_unread
    isPinned: is_pinned
    isMuted: display.notification_settings.mute_for > 0

    openMenuOnPressAndHold: true//chat_id != overviewPage.ownUserId

    onPressAndHold: {
        contextMenuLoader.active = true;
    }

    Loader {
        id: contextMenuLoader
        active: false
        asynchronous: true
        onStatusChanged: {
            if(status === Loader.Ready) {
                listItem.menu = item;
                listItem.openMenu();
            }
        }
        sourceComponent: Component {
            ContextMenu {
                MenuItem {
                    visible: unread_count > 0 || unread_reaction_count > 0 || unread_mention_count > 0
                    onClicked: {
                        tdLibWrapper.viewMessage(chat_id, display.last_message.id, true);
                        tdLibWrapper.readAllChatMentions(chat_id);
                        tdLibWrapper.readAllChatReactions(chat_id);
                        tdLibWrapper.toggleChatIsMarkedAsUnread(chat_id, false);
                    }
                    text: qsTr("Mark all messages as read")
                }

                MenuItem {
                    visible: unread_count === 0 && unread_reaction_count === 0 && unread_mention_count === 0
                    onClicked: {
                        tdLibWrapper.toggleChatIsMarkedAsUnread(chat_id, !is_marked_as_unread);
                    }
                    text: is_marked_as_unread ? qsTr("Mark chat as read") : qsTr("Mark chat as unread")
                }

                MenuItem {
                    onClicked: {
                        tdLibWrapper.toggleChatIsPinned(chat_id, !is_pinned);
                    }
                    text: is_pinned ? qsTr("Unpin chat") : qsTr("Pin chat")
                }

                MenuItem {
                    visible: chat_id != listItem.ownUserId
                    onClicked: {
                        var newNotificationSettings = display.notification_settings;
                        if (newNotificationSettings.mute_for > 0) {
                            newNotificationSettings.mute_for = 0;
                        } else {
                            newNotificationSettings.mute_for = 6666666;
                        }
                        newNotificationSettings.use_default_mute_for = false;
                        tdLibWrapper.setChatNotificationSettings(chat_id, newNotificationSettings);
                    }
                    text: display.notification_settings.mute_for > 0 ? qsTr("Unmute chat") : qsTr("Mute chat")
                }

                MenuItem {
                    onClicked: {
                        if(pageStack.depth > 2) {
                            pageStack.pop(pageStack.find( function(page){ return(page._depth === 0)} ), PageStackAction.Immediate);
                        }

                        pageStack.push(Qt.resolvedUrl("../pages/ChatInformationPage.qml"), { "chatInformation" : display});
                    }
                    text: model.display.type['@type'] === "chatTypePrivate" ? qsTr("User Info") : qsTr("Group Info")
                }
            }
        }
    }

}
