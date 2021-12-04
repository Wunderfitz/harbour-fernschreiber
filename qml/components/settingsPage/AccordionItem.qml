/*
    Copyright (C) 2021 Sebastian J. Wolf and other contributors

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

Item {
    id: area
    width: parent.width
    height: button.height + content.height
    property alias icon: image
    property alias text: label.text
    property alias asynchronous: content.asynchronous
    property bool expanded: false
    default property alias els: content.sourceComponent
    states: [
        State {
            when: area.expanded
            PropertyChanges { target: image; rotation: 90 }
            PropertyChanges { target: content; height: content.implicitHeight + Theme.paddingLarge; opacity: 1.0 }
        }
    ]
    transitions: Transition {
        to: "*"
        enabled: area.parent.animate
        NumberAnimation { target: content; properties: "height, opacity"; duration: 200}
        NumberAnimation { target: image; properties: "rotation"; duration: 200}
    }
    Connections {
        target: area.parent
        onSetActiveArea: {
            var expand = (activeAreaTitle === area.text);
            if(area.expanded && !expand && area.parent.scrollUpFlickable) {
                area.parent.scrollUpFlickable(content.implicitHeight + Theme.paddingLarge);
            }

            area.expanded = expand;
        }
    }
    BackgroundItem {
        id: button
        height: Theme.itemSizeMedium
        onClicked: {
            area.parent.animate = true;
            area.parent.setActiveArea(area.expanded ? -1 : area.text)
        }
        Rectangle {
            anchors.fill: parent
            gradient: Gradient {
                GradientStop { position: 0.0; color: Theme.rgba(Theme.highlightBackgroundColor, 0.15) }
                GradientStop { position: 1.0; color: "transparent" }
            }
        }
        Label {
            id: label
            anchors {
                left: parent.left
                right: image.left
                verticalCenter: parent.verticalCenter
                leftMargin: Theme.horizontalPageMargin + Theme.paddingLarge
                rightMargin: Theme.paddingMedium
            }
            horizontalAlignment: Text.AlignRight
            truncationMode: TruncationMode.Fade
            color: button.highlighted ? Theme.highlightColor : Theme.primaryColor
            textFormat: Text.PlainText
        }
        HighlightImage {
            id: image
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
                rightMargin: Theme.horizontalPageMargin
            }
            width: visible ? Theme.iconSizeMedium : 0
            highlighted: parent.highlighted
            source: "image://theme/icon-m-left"
            rotation: -90
        }
    }
    Loader {
        id: content
        width: parent.width
        height: 0
        opacity: 0
        anchors.top: button.bottom
        asynchronous: true
        clip: true
    }
}
