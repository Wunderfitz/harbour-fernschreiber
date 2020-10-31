import QtQuick 2.6
import Sailfish.Silica 1.0

import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions

PhotoTextsListItem {
    id: listItem
    pictureThumbnail {
//        forceElementUpdate: overviewPage.chatListCreated
        photoData: photo_small
//                            replacementStringHint: primaryText.text
    }
    property int ownUserId

    // chat title
    primaryText.text: title ? Emoji.emojify(title, Theme.fontSizeMedium) + ( display.notification_settings.mute_for > 0 ? Emoji.emojify(" 🔇", Theme.fontSizeMedium) : "" ) : qsTr("Unknown")
    // last user
    prologSecondaryText.text: is_channel ? "" : ( last_message_sender_id ? ( last_message_sender_id !== ownUserId ? Emoji.emojify(Functions.getUserName(tdLibWrapper.getUserInformation(last_message_sender_id)), primaryText.font.pixelSize) : qsTr("You") ) : qsTr("Unknown") )
    // last message
    secondaryText.text: last_message_text ? Emoji.emojify(last_message_text, Theme.fontSizeExtraSmall) : qsTr("Unknown")
    // message date
    tertiaryText.text: last_message_date ? Functions.getDateTimeElapsed(last_message_date) : qsTr("Unknown")
    unreadCount: unread_count

    openMenuOnPressAndHold: true//chat_id != overviewPage.ownUserId
    menu: ContextMenu {
        MenuItem {
            visible: unread_count > 0
            onClicked: {
                tdLibWrapper.viewMessage(chat_id, display.last_message.id, true);
            }
            text: qsTr("Mark all messages as read")
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
                tdLibWrapper.setChatNotificationSettings(chat_id, newNotificationSettings);
            }
            text: display.notification_settings.mute_for > 0 ? qsTr("Unmute Chat") : qsTr("Mute Chat")
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
