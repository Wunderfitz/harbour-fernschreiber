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
import QtMultimedia 5.6
import WerkWolf.Fernschreiber 1.0

InlineQueryResult {
    id: queryResultItem


    TDLibFile {
        id: file
        tdlib: tdLibWrapper
        autoLoad: true
    }

    Loader {
        asynchronous: true
        active: file.isDownloadingCompleted
        anchors.fill: parent
        opacity: item && item.status === Image.Ready ? 1.0 : 0.0
        Behavior on opacity { FadeAnimation {} }
        sourceComponent: Component {
            Image {
                id: image
                source: file.path
                asynchronous: true
                clip: true
                fillMode: Image.PreserveAspectCrop
                layer.enabled: queryResultItem.pressed
                layer.effect: PressEffect { source: image }
            }
        }
    }
    Component.onCompleted: {
        if (model.photo) {
            // Check first which size fits best...
            var photo
            for (var i = 0; i < model.photo.sizes.length; i++) {
                photo = model.photo.sizes[i].photo
                if (model.photo.sizes[i].width >= queryResultItem.width) {
                    break
                }
            }
            if (photo) {
                file.fileInformation = photo
            }
        }
    }
}
