/*
    Copyright (C) 2020-21 Sebastian J. Wolf and other contributors

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
import "./messageContent"
import "../js/functions.js" as Functions
import "../js/twemoji.js" as Emoji
import "../js/debug.js" as Debug

Flickable {
    id: stickerSetOverlayFlickable
    anchors.fill: parent
    boundsBehavior: Flickable.StopAtBounds
    contentHeight: stickerSetContentColumn.height
    clip: true

    property string stickerSetId;
    property var stickerSet;
    signal requestClose;

    Component.onCompleted: {
        if (!stickerManager.hasStickerSet(stickerSetId)) {
            tdLibWrapper.getStickerSet(stickerSetId);
        } else {
            stickerSet = stickerManager.getStickerSet(stickerSetId);
        }
    }

    Connections {
        target: tdLibWrapper
        onStickerSetReceived: {
            if (stickerSet.id === stickerSetOverlayFlickable.stickerSetId) {
                stickerSetOverlayFlickable.stickerSet = stickerSet;
            }
        }
        onOkReceived: {
            if (request === "installStickerSet") {
                appNotification.show(qsTr("Sticker set successfully installed!"));
                installSetButton.visible = false;
                tdLibWrapper.getInstalledStickerSets();
            }
            if (request === "removeStickerSet") {
                appNotification.show(qsTr("Sticker set successfully removed!"));
                installSetButton.visible = true;
                tdLibWrapper.getInstalledStickerSets();
            }
        }
    }

    Rectangle {
        id: stickerSetContentBackground
        color: Theme.overlayBackgroundColor
        opacity: 0.7
        anchors.fill: parent
        MouseArea {
            anchors.fill: parent
            onClicked: {
                stickerSetOverlayFlickable.requestClose();
            }
        }
    }

    Column {
        id: stickerSetContentColumn
        spacing: Theme.paddingMedium
        width: parent.width
        height: parent.height

        Row {
            id: stickerSetTitleRow
            width: parent.width - ( 2 * Theme.horizontalPageMargin )
            height: overlayStickerTitleText.height + ( 2 * Theme.paddingMedium )
            anchors.horizontalCenter: parent.horizontalCenter

            Label {
                id: overlayStickerTitleText

                width: parent.width - installSetButton.width - closeSetButton.width
                text: stickerSet.title
                font.pixelSize: Theme.fontSizeExtraLarge
                font.weight: Font.ExtraBold
                maximumLineCount: 1
                truncationMode: TruncationMode.Fade
                textFormat: Text.StyledText
                anchors.verticalCenter: parent.verticalCenter
            }

            IconButton {
                id: installSetButton
                icon.source: "image://theme/icon-m-add"
                anchors.verticalCenter: parent.verticalCenter
                visible: !stickerManager.isStickerSetInstalled(stickerSet.id)
                onClicked: {
                    tdLibWrapper.changeStickerSet(stickerSet.id, true);
                }
            }

            IconButton {
                id: removeSetButton
                icon.source: "image://theme/icon-m-remove"
                anchors.verticalCenter: parent.verticalCenter
                visible: !installSetButton.visible
                onClicked: {
                    tdLibWrapper.changeStickerSet(stickerSet.id, false);
                }
            }

            IconButton {
                id: closeSetButton
                icon.source: "image://theme/icon-m-clear"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    stickerSetOverlayFlickable.requestClose();
                }
            }
        }

        SilicaGridView {
            id: stickerSetGridView

            width: parent.width - ( 2 * Theme.horizontalPageMargin )
            height: parent.height - stickerSetTitleRow.height - Theme.paddingMedium
            anchors.horizontalCenter: parent.horizontalCenter

            cellWidth: chatPage.isLandscape ? (width / 5) : (width / 3);
            cellHeight: cellWidth

            visible: count > 0

            clip: true

            model: stickerSet.stickers
            delegate: Item {
                width: stickerSetGridView.cellWidth - Theme.paddingSmall
                height: stickerSetGridView.cellHeight - Theme.paddingSmall

                TDLibThumbnail {
                    id: singleStickerThumbnail
                    thumbnail: modelData.thumbnail
                    anchors.fill: parent
                }

                Label {
                    font.pixelSize: Theme.fontSizeSmall
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    text: Emoji.emojify(modelData.emoji, font.pixelSize)
                }

            }

            VerticalScrollDecorator {}
        }

    }

}
