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
import "../components"
import "../js/twemoji.js" as Emoji

Item {
    id: stickerPickerOverlayItem
    anchors.fill: parent

    property variant recentStickers: stickerManager.getRecentStickers()
    property variant installedStickerSets: stickerManager.getInstalledStickerSets()

    Rectangle {
        id: stickerPickerOverlayBackground
        anchors.fill: parent

        color: Theme.overlayBackgroundColor
        opacity: 0.7
    }
    Flickable {
        id: stickerPickerFlickable
        anchors.fill: parent
        anchors.margins: Theme.paddingMedium
        contentHeight: stickerPickerColumn.height
        clip: true

        Column {
            id: stickerPickerColumn
            spacing: Theme.paddingMedium
            width: stickerPickerFlickable.width
            Text {
                font.pixelSize: Theme.fontSizeMedium
                font.bold: true
                color: Theme.primaryColor
                width: parent.width
                maximumLineCount: 1
                elide: Text.ElideRight
                text: qsTr("Recently used")
            }
            Flickable {
                width: parent.width
                height: recentStickersRow.height + Theme.paddingSmall
                anchors.horizontalCenter: parent.horizontalCenter
                contentWidth: recentStickersRow.width
                clip: true

                Row {
                    id: recentStickersRow
                    spacing: Theme.paddingMedium
                    Repeater {
                        model: stickerPickerOverlayItem.recentStickers

                        Item {
                            height: singleRecentStickerRow.height
                            width: singleRecentStickerRow.width

                            Row {
                                id: singleRecentStickerRow
                                spacing: Theme.paddingSmall
                                Image {
                                    source: modelData.sticker.local.path
                                    width: Theme.itemSizeMedium
                                    height: Theme.itemSizeMedium
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    tdLibWrapper.sendStickerMessage(chatInformation.id, modelData.sticker.local.path);
                                    stickerPickerOverlayItem.visible = false;
                                    attachmentOptionsRow.visible = false;
                                }
                            }
                        }

                    }
                }
            }
            Repeater {
                model: stickerPickerOverlayItem.installedStickerSets
                width: stickerPickerFlickable.width
                Column {
                    spacing: Theme.paddingMedium
                    width: parent.width
                    Text {
                        font.pixelSize: Theme.fontSizeMedium
                        font.bold: true
                        color: Theme.primaryColor
                        width: parent.width
                        maximumLineCount: 1
                        elide: Text.ElideRight
                        text: modelData.title
                    }
                    Flickable {
                        width: parent.width
                        height: installedStickerSetRow.height + Theme.paddingSmall
                        anchors.horizontalCenter: parent.horizontalCenter
                        contentWidth: installedStickerSetRow.width
                        clip: true
                        Row {
                            id: installedStickerSetRow
                            spacing: Theme.paddingMedium

                            Repeater {
                                model: modelData.stickers

                                Item {
                                    width: Theme.itemSizeMedium
                                    height: Theme.itemSizeMedium

                                    Component.onCompleted: {
                                        if (!modelData.sticker.local.is_downloading_completed) {
                                            tdLibWrapper.downloadFile(modelData.sticker.id);
                                        }
                                    }

                                    Connections {
                                        target: tdLibWrapper
                                        onFileUpdated : {
                                            if (fileInformation.local.is_downloading_completed) {
                                                if (fileId === modelData.sticker.id) {
                                                    singleStickerImage.source = fileInformation.local.path;
                                                }
                                            }
                                        }
                                    }

                                    Image {
                                        id: singleStickerImage
                                        source: modelData.sticker.local.is_downloading_completed ? modelData.sticker.local.path : ""
                                        anchors.fill: parent
                                        visible: modelData.sticker.local.is_downloading_completed
                                    }
                                    Text {
                                        font.pixelSize: Theme.fontSizeLarge
                                        color: Theme.primaryColor
                                        anchors.fill: parent
                                        maximumLineCount: 1
                                        elide: Text.ElideRight
                                        text: Emoji.emojify(modelData.emoji, font.pixelSize)
                                        visible: !modelData.sticker.local.is_downloading_completed
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            tdLibWrapper.sendStickerMessage(chatInformation.id, modelData.sticker.local.path);
                                            stickerPickerOverlayItem.visible = false;
                                            attachmentOptionsRow.visible = false;
                                        }
                                    }
                                }

                            }
                        }
                    }


                }
            }

        }
    }
}

