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
import QtGraphicalEffects 1.0

Loader {
    id: loader
    property var minithumbnail
    property bool highlighted
    anchors.fill: parent
    active: !!minithumbnail
    visible: active
    sourceComponent: Component {
        Item {
            Image {
                id: minithumbnailImage
                anchors.fill: parent
                source: "data:image/jpg;base64,"+minithumbnail.data
                asynchronous: true
                fillMode: tdLibImage.fillMode
                opacity: status === Image.Ready ? 1.0 : 0.0
                cache: false
                visible: opacity > 0
                Behavior on opacity { FadeAnimation {} }

                layer {
                    enabled: loader.highlighted
                    effect: PressEffect { source: minithumbnailImage }
                }
            }

            FastBlur {
                anchors.fill: parent
                source: minithumbnailImage
                radius: Theme.paddingLarge
            }
        }
    }
}
