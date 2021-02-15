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

    property var recentStickers: stickerManager.getRecentStickers();
    property var installedStickerSets: stickerManager.getInstalledStickerSets();

    Connections {
        target: tdLibWrapper
        onOkReceived: {
            if (request === "removeStickerSet") {
                appNotification.show(qsTr("Sticker set successfully removed!"));
                tdLibWrapper.getInstalledStickerSets();
            }
        }
    }

    Connections {
        target: stickerManager
        onStickerSetsReceived: {
            installedStickerSets = stickerManager.getInstalledStickerSets();
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
                    id: stickerSetColumn

                    property bool isExpanded: false
                    function toggleDisplaySet() {
                        stickerSetColumn.isExpanded = !stickerSetColumn.isExpanded;
                        stickerSetLoader.active = stickerSetColumn.isExpanded;
                        if (stickerSetLoader.active) {
                            stickerSetLoader.myStickerSet = modelData.stickers;
                        } else {
                            stickerSetLoader.myStickerSet = ({});
                        }
                    }

                    spacing: Theme.paddingMedium
                    width: parent.width
                    Row {
                        id: stickerSetTitleRow
                        width: parent.width
                        height: stickerSetThumbnail.height + ( 2 * Theme.paddingSmall )
                        spacing: Theme.paddingMedium

                        TDLibThumbnail {
                            id: stickerSetThumbnail
                            thumbnail: modelData.thumbnail ? modelData.thumbnail : modelData.stickers[0].thumbnail
                            anchors.verticalCenter: parent.verticalCenter
                            width: Theme.itemSizeMedium
                            height: Theme.itemSizeMedium
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    toggleDisplaySet();
                                }
                            }
                        }

                        Label {
                            id: setTitleText
                            font.pixelSize: Theme.fontSizeLarge
                            font.bold: true
                            width: parent.width - removeSetButton.width - expandSetButton.width - stickerSetThumbnail.width - ( 3 * Theme.paddingMedium )
                            anchors.verticalCenter: parent.verticalCenter
                            maximumLineCount: 1
                            truncationMode: TruncationMode.Fade
                            text: modelData.title
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    toggleDisplaySet();
                                }
                            }
                        }

                        IconButton {
                            id: expandSetButton
                            icon.source: stickerSetColumn.isExpanded ? "image://theme/icon-m-up" : "image://theme/icon-m-down"
                            anchors.verticalCenter: parent.verticalCenter
                            onClicked: {
                                toggleDisplaySet();
                            }
                        }

                        IconButton {
                            id: removeSetButton
                            icon.source: "image://theme/icon-m-remove"
                            anchors.verticalCenter: parent.verticalCenter
                            onClicked: {
                                var stickerSetId = modelData.id;
                                Remorse.popupAction(chatPage, qsTr("Removing sticker set"), function() {
                                    tdLibWrapper.changeStickerSet(stickerSetId, false);
                                });
                            }
                        }

                    }

                    Loader {
                        id: stickerSetLoader
                        width: parent.width
                        active: false
                        height: active ? Theme.itemSizeExtraLarge : 0

                        property var myStickerSet

                        sourceComponent: Component {
                            SilicaGridView {
                                id: installedStickerSetGridView
                                width: parent.width
                                height: parent.height
                                cellWidth: Theme.itemSizeExtraLarge;
                                cellHeight: Theme.itemSizeExtraLarge;
                                visible: count > 0
                                clip: true
                                flow: GridView.FlowTopToBottom

                                model: stickerSetLoader.myStickerSet
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
    }

}

