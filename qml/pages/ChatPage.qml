/*
    Copyright (C) 2020 Sebastian J. Wolf

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
import QtQuick 2.5
import QtGraphicalEffects 1.0
import QtMultimedia 5.0
import Sailfish.Silica 1.0
import WerkWolf.Fernschreiber 1.0
import "../components"
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions

Page {
    id: chatPage
    allowedOrientations: Orientation.All

    property bool loading: true;
    property variant chatInformation;

    function initializePage() {
        chatPage.loading = false;
    }

    Component.onCompleted: {
        initializePage();
    }

    SilicaFlickable {
        id: chatContainer
        contentHeight: parent.height
        contentWidth: parent.width
        anchors.fill: parent
        visible: !chatPage.loading

        Column {
            id: chatColumn
            width: parent.width
            height: parent.height
            spacing: Theme.paddingMedium

            Row {
                id: headerRow
                width: parent.width - (3 * Theme.horizontalPageMargin)
                height: chatOverviewColumn.height + Theme.horizontalPageMargin
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: Theme.paddingMedium

                ProfileThumbnail {
                    id: chatPictureThumbnail
                    photoData: (typeof chatInformation.photo !== "undefined") ? chatInformation.photo.small : ""
                    replacementStringHint: chatNameText.text
                    width: chatOverviewColumn.height
                    height: chatOverviewColumn.height
                    anchors.bottom: parent.bottom
                }

                Column {
                    id: chatOverviewColumn
                    width: parent.width - chatPictureThumbnail.width - Theme.paddingMedium
                    anchors.bottom: parent.bottom
                    Text {
                        id: chatNameText
                        text: chatInformation.title !== "" ? Emoji.emojify(chatInformation.title, font.pixelSize) : qsTr("Unknown")
                        textFormat: Text.StyledText
                        font.pixelSize: Theme.fontSizeLarge
                        font.family: Theme.fontFamilyHeading
                        color: Theme.highlightColor
                        elide: Text.ElideRight
                        width: parent.width
                        maximumLineCount: 1
                        horizontalAlignment: Text.AlignRight
                        onTruncatedChanged: {
                            // There is obviously a bug in QML in truncating text with images.
                            // We simply remove Emojis then...
                            if (truncated) {
                                text = text.replace(/\<img [^>]+\/\>/g, "");
                            }
                        }
                    }
                    Text {
                        id: chatStatusText
                        text: "This will become a status bar..."
                        textFormat: Text.StyledText
                        font.pixelSize: Theme.fontSizeSmall
                        font.family: Theme.fontFamilyHeading
                        color: Theme.secondaryColor
                        elide: Text.ElideRight
                        width: parent.width
                        maximumLineCount: 1
                        horizontalAlignment: Text.AlignRight
                    }
                }
            }

            SilicaListView {
                id: chatView

                width: parent.width
                height: parent.height - ( 2 * Theme.paddingMedium ) - headerRow.height - newMessageRow.height

                clip: true
                // visible: count > 0

                // model: chatListModel
                delegate: ListItem {

                    id: chatItem

                    //contentHeight: chatListRow.height + chatListSeparator.height + 2 * Theme.paddingMedium
                    contentWidth: parent.width

                }

                VerticalScrollDecorator {}
            }

            Row {
                id: newMessageRow
                width: parent.width - Theme.horizontalPageMargin
                height: sendMessageColumn.height + ( 2 * Theme.paddingLarge )
                anchors.left: parent.left
                spacing: Theme.paddingMedium
                Column {
                    id: sendMessageColumn
                    width: parent.width - Theme.fontSizeMedium - ( 2 * Theme.paddingMedium )
                    anchors.verticalCenter: parent.verticalCenter
                    TextArea {
                        id: newMessageTextField
                        width: parent.width
                        font.pixelSize: Theme.fontSizeSmall
                        placeholderText: qsTr("Your message")
                        labelVisible: false
                    }
                }

                Column {
                    width: Theme.fontSizeMedium
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: Theme.paddingLarge
                    IconButton {
                        id: newMessageSendButton
                        icon.source: "image://theme/icon-m-chat"
                        anchors.horizontalCenter: parent.horizontalCenter
                        onClicked: {
                            // Send message
                        }
                    }
                }
            }
        }
    }

}
