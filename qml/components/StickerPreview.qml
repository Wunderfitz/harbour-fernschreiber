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
import WerkWolf.Fernschreiber 1.0

Item {
    property ListItem messageListItem

    readonly property var stickerData: messageListItem.myMessage.content.sticker;
    readonly property bool animated: stickerData.is_animated
    readonly property bool stickerVisible: staticStickerLoader.item ? staticStickerLoader.item.visible :
        animatedStickerLoader.item ? animatedStickerLoader.item.visible : false

    implicitWidth: stickerData.width
    implicitHeight: stickerData.height

    TDLibFile {
        id: file
        tdlib: tdLibWrapper
        fileInformation: stickerData.sticker
        autoLoad: true
    }

    Item {
        width: stickerData.width
        height: stickerData.height
        // (centered in image mode, text-like in sticker mode)
        x: appSettings.showStickersAsImages ? (parent.width - width)/2 :
            messageListItem.isOwnMessage ? (parent.width - width) : 0
        anchors.verticalCenter: parent.verticalCenter

        Loader {
            id: animatedStickerLoader
            anchors.fill: parent
            active: animated
            sourceComponent: Component {
                AnimatedImage {
                    anchors.fill: parent
                    source: file.path
                    asynchronous: true
                    paused: !Qt.application.active
                    cache: false
                }
            }
        }

        Loader {
            id: staticStickerLoader
            anchors.fill: parent
            active: !animated
            sourceComponent: Component {
                Image {
                    anchors.fill: parent
                    source: file.path
                    fillMode: Image.PreserveAspectFit
                    autoTransform: true
                    asynchronous: true
                    visible: opacity > 0
                    opacity: status === Image.Ready ? 1 : 0
                    Behavior on opacity { FadeAnimation {} }
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
    }

    Timer {
        id: placeHolderDelayTimer
        interval: 1000
        running: true
    }
}
