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

    property var recentStickers: stickerManager.getRecentStickers()
    property var installedStickerSets: stickerManager.getInstalledStickerSets()
    property bool pickerLoaded: false

    signal stickerPicked(var stickerId)

    Timer {
        id: stickerPickerLoadedTimer
        interval: 100
        running: true
        repeat: false
        onTriggered: {
            stickerPickerOverlayItem.pickerLoaded = true;
        }
    }

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

        opacity: stickerPickerOverlayItem.pickerLoaded ? 1 : 0
        Behavior on opacity { NumberAnimation {} }
        visible: stickerPickerOverlayItem.pickerLoaded

        contentHeight: stickerPickerColumn.height
        clip: true

        Column {
            id: stickerPickerColumn
            spacing: Theme.paddingMedium
            width: stickerPickerFlickable.width
            Label {
                font.pixelSize: Theme.fontSizeMedium
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

                    Image {
                        source: modelData.thumbnail.photo.local.path
                        anchors.fill: parent
                        asynchronous: true
                        onStatusChanged: {
                            if (status === Image.Ready) {
                                stickerPickerLoadedTimer.restart();
                            }
                        }
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
                    Label {
                        font.pixelSize: Theme.fontSizeMedium
                        font.bold: true
                        width: parent.width
                        maximumLineCount: 1
                        truncationMode: TruncationMode.Fade
                        text: modelData.title
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

                            Image {
                                id: singleStickerImage
                                source: modelData.thumbnail.photo.local.is_downloading_completed ? modelData.thumbnail.photo.local.path : ""
                                anchors.fill: parent
                                visible: modelData.thumbnail.photo.local.is_downloading_completed
                                asynchronous: true
                                onStatusChanged: {
                                    if (status === Image.Ready) {
                                        stickerPickerLoadedTimer.restart();
                                    }
                                }
                            }
                            Label {
                                font.pixelSize: Theme.fontSizeHuge
                                anchors.fill: parent
                                maximumLineCount: 1
                                truncationMode: TruncationMode.Fade
                                text: Emoji.emojify(modelData.emoji, font.pixelSize)
                                visible: !modelData.thumbnail.photo.local.is_downloading_completed
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

    Column {
        anchors.centerIn: parent
        width: parent.width
        spacing: Theme.paddingMedium

        opacity: stickerPickerOverlayItem.pickerLoaded ? 0 : 1
        Behavior on opacity { NumberAnimation {} }
        visible: !stickerPickerOverlayItem.pickerLoaded

        InfoLabel {
            id: loadingLabel
            text: qsTr("Loading stickers...")
        }

        BusyIndicator {
            id: loadingBusyIndicator
            anchors.horizontalCenter: parent.horizontalCenter
            running: !stickerPickerOverlayItem.pickerLoaded
            size: BusyIndicatorSize.Large
        }
    }

}

