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
    Component {
        id: stickerComponent
        BackgroundItem {
           id: stickerSetItem
           width: Theme.itemSizeExtraLarge
           height: Theme.itemSizeExtraLarge

           onClicked: stickerPickerOverlayItem.stickerPicked(modelData.sticker.remote.id)

           TDLibThumbnail {
               thumbnail: modelData.thumbnail
               anchors.fill: parent
               highlighted: stickerSetItem.highlighted
           }

           Label {
               font.pixelSize: Theme.fontSizeSmall
               anchors.right: parent.right
               anchors.bottom: parent.bottom
               text: Emoji.emojify(modelData.emoji, font.pixelSize)
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

    SilicaListView {
        id: stickerPickerListView
        anchors.fill: parent
        clip: true

        model: stickerPickerOverlayItem.installedStickerSets

        header: Column {
            spacing: Theme.paddingSmall
            width: stickerPickerListView.width
            height: recentStickersGridView.count > 0 ? ( Theme.fontSizeLarge + Theme.itemSizeExtraLarge + 4 * Theme.paddingSmall ) : 0
            topPadding: Theme.paddingSmall
            Label {
                font.pixelSize: Theme.fontSizeLarge
                font.bold: true
                width: recentStickersGridView.width
                leftPadding: Theme.paddingMedium
                visible: recentStickersGridView.count > 0
                maximumLineCount: 1
                truncationMode: TruncationMode.Fade
                text: qsTr("Recently used")
            }
            SilicaGridView {
                id: recentStickersGridView
                width: stickerPickerListView.width
                height: Theme.itemSizeExtraLarge + Theme.paddingSmall
                cellWidth: Theme.itemSizeExtraLarge;
                cellHeight: Theme.itemSizeExtraLarge;
                visible: count > 0
                clip: true
                flow: GridView.FlowTopToBottom

                model: stickerPickerOverlayItem.recentStickers
                delegate: stickerComponent

                HorizontalScrollDecorator {}

            }
        }
        delegate: Column {
            id: stickerSetColumn

            property bool isExpanded: false
            function toggleDisplaySet() {
                stickerSetColumn.isExpanded = !stickerSetColumn.isExpanded;
                if (stickerSetColumn.isExpanded) {
                    stickerSetLoader.myStickerSet = modelData.stickers;
                }
            }

            spacing: Theme.paddingSmall
            width: parent.width

            Row {
                id: stickerSetTitleRow
                width: parent.width
                height: Theme.itemSizeMedium + ( 2 * Theme.paddingSmall )
                spacing: Theme.paddingMedium
                BackgroundItem {
                    id: stickerSetToggle
                    width: parent.width - removeSetButton.width - Theme.paddingMedium * 2
                    height: parent.height

                    onClicked: {
                        toggleDisplaySet();
                    }
                    TDLibThumbnail {
                        id: stickerSetThumbnail
                        thumbnail: modelData.thumbnail ? modelData.thumbnail : modelData.stickers[0].thumbnail
                        anchors {
                            left: parent.left
                            verticalCenter: parent.verticalCenter
                            leftMargin: Theme.paddingMedium
                        }
                        width: Theme.itemSizeMedium
                        height: Theme.itemSizeMedium
                        highlighted: stickerSetToggle.down
                    }

                    Label {
                        id: setTitleText
                        font.pixelSize: Theme.fontSizeLarge
                        font.bold: true

                        anchors {
                            left: stickerSetThumbnail.right
                            right: expandSetButton.left
                            verticalCenter: parent.verticalCenter
                            margins: Theme.paddingSmall
                        }
                        truncationMode: TruncationMode.Fade
                        text: modelData.title
                    }

                    Icon {
                        id: expandSetButton
                        source: stickerSetColumn.isExpanded ? "image://theme/icon-m-up" : "image://theme/icon-m-down"
                        anchors {
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                            rightMargin: Theme.paddingMedium
                        }
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
                active: stickerSetColumn.isExpanded || height > 0
                height: stickerSetColumn.isExpanded ? Theme.itemSizeExtraLarge + Theme.paddingSmall : 0
                opacity: stickerSetColumn.isExpanded ? 1.0 : 0.0

                Behavior on height {
                    NumberAnimation { duration: 200 }
                }
                Behavior on opacity {
                    NumberAnimation { duration: 200 }
                }

                property var myStickerSet
                onActiveChanged: {
                    if(!active) {
                        myStickerSet = ({});
                    }
                }

                sourceComponent: Component {
                    SilicaListView {
                        id: installedStickerSetGridView
                        width: stickerSetLoader.width
                        height: stickerSetLoader.height

                        orientation: Qt.Horizontal
                        visible: count > 0

                        model: stickerSetLoader.myStickerSet
                        delegate: stickerComponent

                        HorizontalScrollDecorator {}
                    }
                }
            }
        }
    }
}
