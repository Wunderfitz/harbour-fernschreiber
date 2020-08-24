/*
    Copyright (C) 2020 Sebastian J. Wolf

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
import QtQuick 2.0
import Sailfish.Silica 1.0

Item {

    id: backgroundProgressIndicator

    property bool withPercentage : false;
    property bool small : false;
    property int progress;

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter

    width: small ? parent.width / 2 : parent.width
    height: small ? parent.height / 2 : parent.height

    Behavior on opacity { NumberAnimation {} }
    visible: progress < 100
    opacity: progress < 100 ? 1 : 0
    ProgressCircle {
        id: imageProgressCircle
        width: withPercentage ? parent.height / 2 : parent.height
        height: withPercentage ? parent.height / 2 : parent.height
        value: backgroundProgressIndicator.progress / 100
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

    }
    Text {
        id: imageProgressText
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.secondaryColor
        text: withPercentage ? qsTr("%1 \%").arg(backgroundProgressIndicator.progress) : qsTr("%1").arg(backgroundProgressIndicator.progress)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        visible: backgroundProgressIndicator.progress < 100 ? true : false
    }

}
