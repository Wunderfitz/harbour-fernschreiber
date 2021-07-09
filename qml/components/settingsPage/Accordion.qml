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


Column {
    width: parent.width
    property SilicaFlickable flickable
    property bool animate: false
    signal setActiveArea(string activeAreaTitle)
    function scrollUpFlickable(amount) {
        if(flickable) {
            flickableAnimation.to = Math.max(0, flickable.contentY - amount);
            flickableAnimation.start()
        }
    }

    NumberAnimation {
        id: flickableAnimation
        target: flickable
        property: "contentY"
        duration: 200
    }
}
