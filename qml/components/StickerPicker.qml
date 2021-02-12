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

Item {
    id: stickerPickerOverlayItem
    anchors.fill: parent

    property var recentStickers
    property var installedStickerSets
    property int setInRemoval: -1

    Component.onCompleted: {
        recentStickers = stickerManager.getRecentStickers();
        installedStickerSets = stickerManager.getInstalledStickerSets();
    }

    Connections {
        target: tdLibWrapper
        onOkReceived: {
            if (request === "removeStickerSet") {
                appNotification.show(qsTr("Sticker set successfully removed!"));
                tdLibWrapper.getInstalledStickerSets();
                console.log("Set in removal: " + setInRemoval);
                if (setInRemoval > -1) {
                    installedStickerSets.splice(setInRemoval, 1);
                    setInRemoval = -1;
                }
            }
        }
    }

    signal stickerPicked(var stickerId)

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
            Label {
                font.pixelSize: Theme.fontSizeLarge
                font.bold: true
                width: parent.width
                maximumLineCount: 1
                truncationMode: TruncationMode.Fade
                text: qsTr("Recently used")
            }

            SilicaGridView {
                id: recentStickersGridView
                width: parent.width
                height: Theme.itemSizeExtraLarge
                cellWidth: Theme.itemSizeExtraLarge;
                cellHeight: Theme.itemSizeExtraLarge;
                visible: count > 0
                clip: true
                flow: GridView.FlowTopToBottom

                model: stickerPickerOverlayItem.recentStickers

                delegate: Item {
                    width: recentStickersGridView.cellWidth
                    height: recentStickersGridView.cellHeight

                    TDLibThumbnail {
                        thumbnail: modelData.thumbnail
                        anchors.fill: parent
                    }

                    Label {
                        font.pixelSize: Theme.fontSizeSmall
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        text: Emoji.emojify(modelData.emoji, font.pixelSize)
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: stickerPickerOverlayItem.stickerPicked(modelData.sticker.remote.id)
                    }

                }

                HorizontalScrollDecorator {}

            }

            Repeater {
                model: stickerPickerOverlayItem.installedStickerSets
                width: stickerPickerFlickable.width
                Column {
                    spacing: Theme.paddingMedium
                    width: parent.width
                    Row {
                        width: parent.width
                        height: Math.max(removeSetButton.height, setTitleText.height) + ( 2 * Theme.paddingSmall )
                        Label {
                            id: setTitleText
                            font.pixelSize: Theme.fontSizeLarge
                            font.bold: true
                            width: parent.width - removeSetButton.width
                            anchors.verticalCenter: parent.verticalCenter
                            maximumLineCount: 1
                            truncationMode: TruncationMode.Fade
                            text: modelData.title
                        }

                        IconButton {
                            id: removeSetButton
                            icon.source: "image://theme/icon-m-remove"
                            anchors.verticalCenter: parent.verticalCenter
                            onClicked: {
                                var stickerSetId = modelData.id;
                                setInRemoval = index;
                                Remorse.popupAction(chatPage, qsTr("Removing sticker set"), function() {
                                    tdLibWrapper.changeStickerSet(stickerSetId, false);
                                });
                            }
                        }
                    }

                    SilicaGridView {
                        id: installedStickerSetGridView
                        width: parent.width
                        height: Theme.itemSizeExtraLarge
                        cellWidth: Theme.itemSizeExtraLarge;
                        cellHeight: Theme.itemSizeExtraLarge;
                        visible: count > 0
                        clip: true
                        flow: GridView.FlowTopToBottom

                        model: modelData.stickers
                        delegate: Item {
                            width: installedStickerSetGridView.cellWidth
                            height: installedStickerSetGridView.cellHeight

                            TDLibThumbnail {
                                thumbnail: modelData.thumbnail
                                anchors.fill: parent
                            }

                            Label {
                                font.pixelSize: Theme.fontSizeSmall
                                anchors.right: parent.right
                                anchors.bottom: parent.bottom
                                text: Emoji.emojify(modelData.emoji, font.pixelSize)
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: stickerPickerOverlayItem.stickerPicked(modelData.sticker.remote.id)
                            }
                        }

                        HorizontalScrollDecorator {}
                    }

                }
            }
        }
    }

}

