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
import "../components"
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions
import "../js/functionsOld.js" as FunctionsOld
import "../js/debug.js" as Debug

Page {
    id: aboutPage
    allowedOrientations: Orientation.All

    property var userInformation : tdLibWrapper.getUserInformation();

    SilicaFlickable {
        id: aboutContainer
        contentHeight: column.height
        anchors.fill: parent

        Column {
            id: column
            width: aboutPage.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("About Fernschreiber")
            }

            Image {
                id: wunderfitzImage
                source: "../../images/fernschreiber.svg"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                sourceSize {
                    width: width
                    height: height
                }
                fillMode: Image.PreserveAspectFit
                asynchronous: true
                width: Math.min(2 * Theme.itemSizeHuge, Math.min(aboutPage.width, aboutPage.height) / 2)
                height: width
            }

            Label {
                text: "Fernschreiber 0.5.1"
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Theme.fontSizeExtraLarge
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Label {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("A Telegram client for Sailfish OS")
                font.pixelSize: Theme.fontSizeSmall
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Label {
                text: qsTr("By Sebastian J. Wolf and <a href=\"https://github.com/Wunderfitz/harbour-fernschreiber#contributions\">other contributors</a>")
                font.pixelSize: Theme.fontSizeSmall
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                linkColor: Theme.highlightColor
                onLinkActivated: Qt.openUrlExternally(link)
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                text: "<a href=\"mailto:sebastian@ygriega.de\">" + qsTr("Send E-Mail") + "</a>"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor

                onLinkActivated: Qt.openUrlExternally("mailto:sebastian@ygriega.de")
            }

            Separator {
                width: parent.width
                color: Theme.primaryColor
                horizontalAlignment: Qt.AlignHCenter
            }

            Label {
                text: qsTr("Licensed under GNU GPLv3")
                font.pixelSize: Theme.fontSizeSmall
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                text: "<a href=\"https://github.com/Wunderfitz/harbour-fernschreiber\">" + qsTr("Sources on GitHub") + "</a>"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor

                onLinkActivated: Qt.openUrlExternally("https://github.com/Wunderfitz/harbour-fernschreiber")
            }

            SectionHeader {
                text: qsTr("About Telegram")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("This product uses the Telegram API but is not endorsed or certified by Telegram.")
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("TDLib version %1").arg(tdLibWrapper.getVersion())
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Loader {
                active: !!aboutPage.userInformation.phone_number
                width: parent.width
                sourceComponent: Component {
                    Column {

                        Text {
                            x: Theme.horizontalPageMargin
                            width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                            horizontalAlignment: Text.AlignHCenter
                            text: qsTr("Logged in as %1").arg(Emoji.emojify(aboutPage.userInformation.first_name + " " + aboutPage.userInformation.last_name, Theme.fontSizeSmall))
                            font.pixelSize: Theme.fontSizeSmall
                            wrapMode: Text.Wrap
                            color: Theme.primaryColor
                            textFormat: Text.StyledText
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                        }

                        ProfileThumbnail {
                            photoData: aboutPage.userInformation.profile_photo.small
                            width: Theme.itemSizeExtraLarge
                            height: Theme.itemSizeExtraLarge
                            replacementStringHint: aboutPage.userInformation.first_name + " " + aboutPage.userInformation.last_name
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                        }

                        Label {
                            x: Theme.horizontalPageMargin
                            width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                            horizontalAlignment: Text.AlignHCenter
                            text: qsTr("Phone number: +%1").arg(aboutPage.userInformation.phone_number)
                            font.pixelSize: Theme.fontSizeSmall
                            wrapMode: Text.Wrap
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                }
            }

            Button {
                id: flickrTosButton
                text: qsTr("Terms of Service")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    Qt.openUrlExternally("https://telegram.org/tos");
                }
            }

            Button {
                id: flickrPrivacyButton
                text: qsTr("Privacy Policy")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    Qt.openUrlExternally("https://telegram.org/privacy")
                }
            }

            SectionHeader {
                text: qsTr("Credits")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                text: qsTr("This project uses the Telegram Database Library (TDLib). Thanks for making it available under the conditions of the Boost Software License 1.0!")
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                text: "<a href=\"https://github.com/tdlib/td\">" + qsTr("Open Telegram Database Library on GitHub") + "</a>"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor

                onLinkActivated: Qt.openUrlExternally("https://github.com/tdlib/td")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                text: qsTr("This project uses twemoji. Copyright 2018 Twitter, Inc. and other contributors. Thanks for making it available under the conditions of the MIT License (coding) and CC-BY 4.0 (graphics)!")
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                text: "<a href=\"https://github.com/twitter/twemoji\">" + qsTr("Open twemoji on GitHub") + "</a>"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor

                onLinkActivated: Qt.openUrlExternally("https://github.com/twitter/twemoji")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                text: qsTr("This project uses rlottie. Copyright 2020 Samsung Electronics Co., Ltd. and other contributors. Thanks for making it available under the conditions of the MIT License!")
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                text: "<a href=\"https://github.com/Samsung/rlottie\">" + qsTr("Open rlottie on GitHub") + "</a>"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor

                onLinkActivated: Qt.openUrlExternally("https://github.com/Samsung/rlottie")
            }

            Label {
                id: separatorLabel
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            VerticalScrollDecorator {}
        }

    }
    Timer {
       id: profileTimer
       interval: 1000
       property bool hasRun
       property var cases: []
       onTriggered: {
           if(cases.length === 0) {
               return;
           }

           if(!hasRun) {
               hasRun = true;
               Debug.profile();
           }
           cases.pop()();

           if(cases.length > 0) {
               restart();
           } else {
               Debug.profileEnd();
           }
       }
   }

   Component.onCompleted: {
       profileTimer.cases.push(function(){
           Functions.compareAndRepeat(
              "getUserName",
              Functions.getUserName,
              FunctionsOld.getUserName,
              [
                  [{first_name: "Test", last_name: "User"}],
                  [{first_name: "Test", last_name: ""}],
                  [{first_name: "Test"}],
                  [{first_name: "", last_name: "User"}],
                  [{last_name: "User"}]
              ],
               800
           )
       });

       var testMessages = [
                   {
                       "content": {
                           "@type": "messageText",
                           "text": {"@type":"formattedText","entities":[{"@type":"textEntity","length":15,"offset":0,"type":{"@type":"textEntityTypeBold"}},{"@type":"textEntity","length":14,"offset":194,"type":{"@type":"textEntityTypeUrl"}},{"@type":"textEntity","length":31,"offset":303,"type":{"@type":"textEntityTypeBold"}},{"@type":"textEntity","length":46,"offset":341,"type":{"@type":"textEntityTypeBold"}},{"@type":"textEntity","length":23,"offset":460,"type":{"@type":"textEntityTypeBold"}}],"text":"Neue Anmeldung. Liebe(r) xxxxx, wir haben eine Anmeldung von einem neuen Gerät bei deinem Konto am 99/99/9999 um 99:99:99 UTC festgestellt.\n\nGerät: Unknown device\nStandort: xxxxxxxxxxxxxx (IP = xxx.xxx.xx.xxx)\n\nWenn du das nicht selbst gewesen bist, so kannst du die die entsprechende Sitzung abmelden: Telegram Einstellungen > Geräte (oder Privatsphäre und Sicherheit > Aktive Sitzungen.\n\nHat sich jemand ohne dein Einverständnis angemeldet, so kannst du die zweistufige Bestätigung in den Einstellungen unter Privatsphäre und Sicherheit aktivieren."}
                       }
                   },
                   {
                       "content": {
                           "@type": "messageText",
                           "text": {"@type":"formattedText","entities":[{"@type":"textEntity","length":12,"offset":0,"type":{"@type":"textEntityTypeBold"}},{"@type":"textEntity","length":15,"offset":20,"type":{"@type":"textEntityTypeBold"}}],"text":"Anmeldecode: 12345. Auf keinen Fall diesen Code anderen geben, selbst wenn sie behaupten zum Telegram-Team zu gehören!\n\nDieser Code kann dazu benutzt werden, um sich mit deinem Konto zu verbinden. Wir fragen den Code niemals für einen anderen Zweck ab.\n\nWenn du den Code nicht durch die Anmeldung eines anderen Gerätes angefordert hast, so kannst du diese Nachricht einfach ignorieren."}
                       }
                   },
                   {
                       "content": {
                           "@type": "messageText",
                           "text": {"@type":"formattedText","entities":[{"@type":"textEntity","length":10,"offset":0,"type":{"@type":"textEntityTypeBold"}},{"@type":"textEntity","length":14,"offset":169,"type":{"@type":"textEntityTypeUrl"}},{"@type":"textEntity","length":18,"offset":240,"type":{"@type":"textEntityTypeBold"}},{"@type":"textEntity","length":36,"offset":263,"type":{"@type":"textEntityTypeBold"}},{"@type":"textEntity","length":21,"offset":390,"type":{"@type":"textEntityTypeBold"}}],"text":"New login. Dear xxxxx, we detected a login into your account from a new device on 99/99/9999 at 99:99:99 UTC.\n\nDevice: Unknown device\nLocation: xxxxxxxxxxxxxxxxxx (IP = xxx.xxx.xx.xxx)\n\nIf this wasn't you, you can terminate that session in Settings > Devices (or Privacy & Security > Active Sessions).\n\nIf you think that somebody logged in to your account against your will, you can enable Two-Step Verification in Privacy and Security settings."}
                       }
                   },
                   {"@type":"message","content":{"@type":"messageText","text":{"@type":"formattedText","entities":[],"text":"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam faucibus erat libero, vitae dictum purus tristique vitae. Praesent lacinia eu augue vitae convallis. Praesent leo & ante, lacinia et efficitur ac, <volutpat in purus. <3 Curabitur nec diam odio. Curabitur interdum mollis libero, nec tincidunt leo vestibulum sit amet. In convallis nisl nibh, a venenatis mi efficitur eu. Integer posuere turpis est, eget congue sapien elementum eget. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Morbi et eleifend."}},"forward_info":{"@type":"messageForwardInfo","date":12323545345,"from_chat_id":0,"from_message_id":0,"origin":{"@type":"messageForwardOriginHiddenUser","sender_name":"USER"}}},
                   {"@type":"message","content":{"@type":"messageSticker","sticker":{"@type":"sticker","emoji":"👍","height":512,"is_animated":true,"is_mask":false,"set_id":"12323545345","sticker":{"@type":"file"},"thumbnail":{},"width":512}}},
                   {"@type":"message","content":{"@type":"messagePoll","poll":{"@type":"poll","id":"12323545345","is_anonymous":false,"is_closed":false,"options":[{"@type":"pollOption","is_being_chosen":false,"is_chosen":true,"text":"Auch ","vote_percentage":100,"voter_count":1},{"@type":"pollOption","is_being_chosen":false,"is_chosen":false,"text":"Januar ","vote_percentage":0,"voter_count":0}],"question":"New","recent_voter_user_ids":[12323545345],"total_voter_count":1,"type":{"@type":"pollTypeRegular","allow_multiple_answers":false}}}},
                   {"@type":"message","content":{"@type":"messageText","text":{"@type":"formattedText","entities":[],"text":"This is not a message, this is just a tribute"}},"date":12323545345,"edit_date":0,"forward_info":{"@type":"messageForwardInfo","date":12323545345,"from_chat_id":0,"from_message_id":0,"origin":{"@type":"messageForwardOriginUser","sender_user_id":12323545345}}}
               ];

       profileTimer.cases.push(function(){
           Functions.compareAndRepeat(
              "getMessageText",
              Functions.getMessageText,
              FunctionsOld.getMessageText,
              [
                           [testMessages[0], true, true],
                           [testMessages[1], true, true],
                           [testMessages[2], true, true],
                           [testMessages[3], true, true],
                           [testMessages[4], true, true],
                           [testMessages[5], true, true],
                           [testMessages[0], true, false],
                           [testMessages[1], true, false],
                           [testMessages[2], true, false],
                           [testMessages[3], true, false],
                           [testMessages[4], true, false],
                           [testMessages[5], true, false],
                           [testMessages[0], false, false],
                           [testMessages[1], false, false],
                           [testMessages[2], false, false],
                           [testMessages[3], false, false],
                           [testMessages[4], false, false],
                           [testMessages[5], false, false],
              ],
              100
           );
       });

       var messageContentPayload = testMessages.filter(function(message) {return message.content["@type"] === "messageText"}).map(function(message) {return [message.content.text]});
       profileTimer.cases.push(function(){
           Functions.compareAndRepeat(
              "enhanceMessageText",
              Functions.enhanceMessageText,
              FunctionsOld.enhanceMessageText,
              messageContentPayload,
                       500
           )
       });

       var enhanceHtmlEntitiesPayload = testMessages.filter(function(message) {return message.content["@type"] === "messageText"}).map(function(message) {return [message.content.text.text]});

       profileTimer.cases.push(function(){
           Functions.compareAndRepeat(
              "enhanceHtmlEntities",
              Functions.enhanceHtmlEntities,
              FunctionsOld.enhanceHtmlEntities,
              enhanceHtmlEntitiesPayload,
                       500
           )
       });

       profileTimer.start();

   }
}
