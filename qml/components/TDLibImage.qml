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
import WerkWolf.Fernschreiber 1.0
import Sailfish.Silica 1.0

import "../js/debug.js" as Debug

Image {
    id: tdLibImage
    property alias fileInformation: file.fileInformation
    readonly property alias file: file
    property bool highlighted

    asynchronous: true
    enabled: !!file.fileId
    fillMode: Image.PreserveAspectCrop
    clip: true
    opacity: status === Image.Ready ? 1.0 : 0.0
    source: enabled && file.isDownloadingCompleted ? file.path : ""
    visible: opacity > 0
    sourceSize {
        width: width
        height: height
    }

    Behavior on opacity { FadeAnimation {} }

    layer {
        enabled: tdLibImage.enabled && tdLibImage.highlighted
        effect: PressEffect { source: tdLibImage }
    }

    TDLibFile {
        id: file
        autoLoad: true
        tdlib: tdLibWrapper
    }
}
