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
    id: voiceNoteOverlayItem
    anchors.fill: parent

    Rectangle {
        id: stickerPickerOverlayBackground
        anchors.fill: parent

        color: Theme.overlayBackgroundColor
        opacity: Theme.opacityHigh
    }

    Flickable {
        id: voiceNoteFlickable
        anchors.fill: parent
        anchors.margins: Theme.paddingMedium

        Behavior on opacity { NumberAnimation {} }

        contentHeight: voiceNoteColumn.height
        clip: true

        Column {
            id: voiceNoteColumn
            spacing: Theme.paddingMedium
            width: voiceNoteFlickable.width

            InfoLabel {
                text: qsTr("Record a Voice Note")
            }

            Image {
                id: recorderImage
                source: "image://theme/icon-l-recorder"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                asynchronous: true
                width: Theme.itemSizeLarge
                height: Theme.itemSizeLarge
            }

        }
    }

}

