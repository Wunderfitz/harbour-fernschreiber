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
import "../"

MessageContentBase {
    height: Math.max(Theme.itemSizeExtraSmall, Math.min(defaultHeight, width / (biggest.width/biggest.height)))
    readonly property int defaultHeight: Math.round(width * 0.66666666)
    readonly property var biggest: rawMessage.content.photo.sizes[rawMessage.content.photo.sizes.length - 1];

    onClicked: {
        pageStack.push(Qt.resolvedUrl("../../pages/ImagePage.qml"), {
            "photoData" : photo.photo,
//            "pictureFileInformation" : photo.fileInformation
        })
    }
    TDLibPhoto {
        id: photo
        anchors.fill: parent
        photo: rawMessage.content.photo
        highlighted: parent.highlighted
    }
    BackgroundImage {
        visible: !rawMessage.content.photo.minithumbnail && photo.image.status !== Image.Ready
    }
}
