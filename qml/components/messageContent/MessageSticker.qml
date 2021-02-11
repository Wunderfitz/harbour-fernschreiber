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
import WerkWolf.Fernschreiber 1.0
import "../"

MessageContentBase {
    readonly property var stickerData: messageListItem ? messageListItem.myMessage.content.sticker : overlayFlickable.overlayMessage.content.sticker;
    readonly property bool animated: stickerData.is_animated && appSettings.animateStickers
    readonly property bool stickerVisible: staticStickerLoader.item ? staticStickerLoader.item.visible :
        animatedStickerLoader.item ? animatedStickerLoader.item.visible : false
    readonly property bool isOwnSticker : messageListItem ? messageListItem.isOwnMessage : overlayFlickable.isOwnMessage
    property real aspectRatio: stickerData.width / stickerData.height

    implicitWidth: stickerData.width
    implicitHeight: stickerData.height

    TDLibFile {
        id: file
        tdlib: tdLibWrapper
        fileInformation: stickerData.sticker
        autoLoad: true
    }

    Item {
        width: Math.min( stickerData.width, parent.width )
        height: width * aspectRatio
        // (centered in image mode, text-like in sticker mode)
        x: appSettings.showStickersAsImages ? (parent.width - width) / 2 :
            isOwnSticker ? (parent.width - width) : 0
        anchors.verticalCenter: parent.verticalCenter

        Loader {
            id: animatedStickerLoader
            anchors.fill: parent
            active: animated
            sourceComponent: Component {
                AnimatedImage {
                    id: animatedSticker
                    anchors.fill: parent
                    source: file.path
                    asynchronous: true
                    paused: !Qt.application.active
                    cache: false
                    layer.enabled: highlighted
                    layer.effect: PressEffect { source: animatedSticker }
                }
            }
        }

        Loader {
            id: staticStickerLoader
            anchors.fill: parent
            active: !animated
            sourceComponent: Component {
                Image {
                    id: staticSticker
                    anchors.fill: parent
                    source: file.path
                    fillMode: Image.PreserveAspectFit
                    autoTransform: true
                    asynchronous: true
                    visible: opacity > 0
                    opacity: status === Image.Ready ? 1 : 0
                    Behavior on opacity { FadeAnimation {} }
                    layer.enabled: highlighted
                    layer.effect: PressEffect { source: staticSticker }
                }
            }
        }

        Loader {
            anchors.fill: parent
            sourceComponent: Component {
                BackgroundImage {}
            }

            active: opacity > 0
            opacity: !stickerVisible && !placeHolderDelayTimer.running ? 0.15 : 0
            Behavior on opacity { FadeAnimation {} }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                stickerSetOverlayLoader.stickerSetId = stickerData.set_id;
                stickerSetOverlayLoader.active = true;
            }
        }
    }

    Timer {
        id: placeHolderDelayTimer
        interval: 1000
        running: true
    }
}
