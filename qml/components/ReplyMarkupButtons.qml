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
import QtQuick 2.6
import Sailfish.Silica 1.0
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions
import "../js/debug.js" as Debug

Column {
    id: replyMarkupButtons
    width: parent.width
    height: childrenRect.height
    spacing: Theme.paddingSmall

    Repeater {
        model: myMessage.reply_markup.rows
        delegate: Row {
            width: parent.width
            height: Theme.itemSizeSmall
            spacing: Theme.paddingSmall
            Repeater {
                id: buttonsRepeater
                model: modelData
                property int itemWidth: replyMarkupButtons.width / count
                delegate: MouseArea {
                    /*
                    Unimplemented callback types:
                        inlineKeyboardButtonTypeBuy
                        inlineKeyboardButtonTypeCallbackGame
                        inlineKeyboardButtonTypeCallbackWithPassword
                        inlineKeyboardButtonTypeLoginUrl
                        inlineKeyboardButtonTypeSwitchInline
                    */
                    property var callbacks: ({
                        inlineKeyboardButtonTypeCallback: function(){
                            tdLibWrapper.getCallbackQueryAnswer(myMessage.chat_id, myMessage.id, {data: modelData.type.data, "@type": "callbackQueryPayloadData"})
                        },

                         inlineKeyboardButtonTypeCallbackGame: function(){
                             tdLibWrapper.getCallbackQueryAnswer(myMessage.chat_id, myMessage.id, {game_short_name: myMessage.content.game.short_name, "@type": "callbackQueryPayloadGame"})
                         },
                        inlineKeyboardButtonTypeUrl: function() {
                            Functions.handleLink(modelData.type.url);
                        },
                        inlineKeyboardButtonTypeSwitchInline: function() {
                            if(modelData.type.in_current_chat) {
                                chatPage.setMessageText("@" + userInformation.username + " "+(modelData.type.query || ""))
                            } else {

                                pageStack.push(Qt.resolvedUrl("../pages/ChatSelectionPage.qml"), {
                                    myUserId: chatPage.myUserId,
                                    payload: { neededPermissions: ["can_send_other_messages"], text:"@" + userInformation.username + " "+(modelData.type.query || "")},
                                    state: "fillTextArea"
                                })
                            }
                        },

                        keyboardButtonTypeText: function() {
                            chatPage.setMessageText(modelData.text, true);
                        }
                    })
                    enabled: !!callbacks[modelData.type["@type"]]
                    height: Theme.itemSizeSmall
                    width: (replyMarkupButtons.width + Theme.paddingSmall) / buttonsRepeater.count - (Theme.paddingSmall)
                    onClicked: {
                        callbacks[modelData.type["@type"]]();
                    }
                    Rectangle {
                        anchors.fill: parent
                        radius: Theme.paddingSmall
                        color: parent.pressed ? Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)
                                          : Theme.rgba(Theme.primaryColor, Theme.opacityFaint)
                        opacity: parent.enabled ? 1.0 : Theme.opacityLow

                        Label {
                            width: Math.min(parent.width - Theme.paddingSmall*2, contentWidth)
                            truncationMode: TruncationMode.Fade
                            text: Emoji.emojify(modelData.text, Theme.fontSizeSmall)
                            color: parent.parent.pressed ? Theme.highlightColor : Theme.primaryColor
                            anchors.centerIn: parent
                            font.pixelSize: Theme.fontSizeSmall
                        }
                        Icon {
                            property var sources: ({
                                                   inlineKeyboardButtonTypeUrl: "../../images/icon-s-link.svg",
                                                   inlineKeyboardButtonTypeSwitchInline: !modelData.type.in_current_chat ? "image://theme/icon-s-repost" : "image://theme/icon-s-edit",
                                                   inlineKeyboardButtonTypeCallbackWithPassword: "image://theme/icon-s-asterisk"
                                                   })
                            visible: !!sources[modelData.type["@type"]]
                            opacity: 0.6
                            source: sources[modelData.type["@type"]] || ""
                            sourceSize: Qt.size(Theme.iconSizeSmall, Theme.iconSizeSmall)
                            highlighted: parent.pressed
                            anchors {
                                right: parent.right
                                top: parent.top
                            }
                        }
                    }

                }
            }
        }
    }
}
