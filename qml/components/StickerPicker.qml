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

Item {
    id: stickerPickerOverlayItem
    anchors.fill: parent

    property var recentStickers: stickerManager.getRecentStickers();
    property var favoriteStickers: stickerManager.getFavoriteStickers();
    property var installedStickerSets: stickerManager.getInstalledStickerSets();
    property Item stickerMenu

    // The menu opens below the full-width row that holds the sticker, as the
    // horizontal sticker lists are only one sticker high
    function openStickerMenu(sticker, menuHost) {
        if (!stickerMenu) {
            stickerMenu = stickerMenuComponent.createObject(stickerPickerOverlayItem);
        }
        stickerMenu.sticker = sticker;
        stickerMenu.isFavorite = stickerManager.isFavoriteSticker(sticker.sticker.remote.id);
        stickerMenu.open(menuHost);
    }

    function menuHeight(menuHost) {
        return stickerMenu && stickerMenu.parent === menuHost ? stickerMenu.height : 0;
    }

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
        onRecentStickersChanged: {
            recentStickers = stickerManager.getRecentStickers();
        }
        onFavoriteStickersChanged: {
            favoriteStickers = stickerManager.getFavoriteStickers();
        }
    }
    Component {
        id: stickerComponent
        StickerPickerItem {
            width: Theme.itemSizeExtraLarge
            height: Theme.itemSizeExtraLarge
            sticker: modelData

            onClicked: stickerPickerOverlayItem.stickerPicked(modelData.sticker.remote.id)
            onPressAndHold: stickerPickerOverlayItem.openStickerMenu(modelData, (GridView.view || ListView.view).menuHost)
        }
    }

    Component {
        id: stickerMenuComponent
        ContextMenu {
            id: stickerContextMenu
            property var sticker
            property bool isFavorite
            MenuItem {
                text: stickerContextMenu.isFavorite ? qsTr("Remove from favorites") : qsTr("Add to favorites")
                onClicked: {
                    if (stickerContextMenu.isFavorite) {
                        tdLibWrapper.removeFavoriteSticker(stickerContextMenu.sticker.sticker.remote.id);
                    } else {
                        tdLibWrapper.addFavoriteSticker(stickerContextMenu.sticker.sticker.remote.id);
                    }
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

    SilicaListView {
        id: stickerPickerListView
        anchors.fill: parent
        clip: true

        model: stickerPickerOverlayItem.installedStickerSets

        header: Column {
            spacing: Theme.paddingSmall
            width: stickerPickerListView.width
            topPadding: Theme.paddingSmall
            bottomPadding: favoriteStickersGridView.count > 0 || recentStickersGridView.count > 0 ? Theme.paddingSmall : 0
            Label {
                font.pixelSize: Theme.fontSizeLarge
                font.bold: true
                width: favoriteStickersGridView.width
                leftPadding: Theme.paddingMedium
                visible: favoriteStickersGridView.count > 0
                maximumLineCount: 1
                truncationMode: TruncationMode.Fade
                text: qsTr("Favorites")
            }
            Item {
                id: favoriteStickersRow
                width: stickerPickerListView.width
                height: favoriteStickersGridView.height + stickerPickerOverlayItem.menuHeight(favoriteStickersRow)
                visible: favoriteStickersGridView.count > 0

                SilicaGridView {
                    id: favoriteStickersGridView
                    property Item menuHost: favoriteStickersRow
                    width: parent.width
                    height: Theme.itemSizeExtraLarge + Theme.paddingSmall
                    cellWidth: Theme.itemSizeExtraLarge;
                    cellHeight: Theme.itemSizeExtraLarge;
                    clip: true
                    flow: GridView.FlowTopToBottom

                    model: stickerPickerOverlayItem.favoriteStickers
                    delegate: stickerComponent

                    HorizontalScrollDecorator {}

                }
            }
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
            Item {
                id: recentStickersRow
                width: stickerPickerListView.width
                height: recentStickersGridView.height + stickerPickerOverlayItem.menuHeight(recentStickersRow)
                visible: recentStickersGridView.count > 0

                SilicaGridView {
                    id: recentStickersGridView
                    property Item menuHost: recentStickersRow
                    width: parent.width
                    height: Theme.itemSizeExtraLarge + Theme.paddingSmall
                    cellWidth: Theme.itemSizeExtraLarge;
                    cellHeight: Theme.itemSizeExtraLarge;
                    clip: true
                    flow: GridView.FlowTopToBottom

                    model: stickerPickerOverlayItem.recentStickers
                    delegate: stickerComponent

                    HorizontalScrollDecorator {}

                }
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
                        fillMode: Image.PreserveAspectFit
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

            Item {
                id: stickerSetRow
                width: parent.width
                height: stickerSetLoader.height + stickerPickerOverlayItem.menuHeight(stickerSetRow)

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
                            property Item menuHost: stickerSetRow
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
}
