/*
    Copyright (C) 2020 Sebastian J. Wolf

    This file is part of Fernschreiber.

    fernschreiber is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    fernschreiber is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Fernschreiber. If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 2.5
import Sailfish.Silica 1.0

CoverBackground {

    id: coverPage

    Image {
        id: backgroundImage
        source: "../../images/background" + ( Theme.colorScheme ? "-black" : "-white" ) + ".png"
        anchors {
            verticalCenter: parent.verticalCenter

            bottom: parent.bottom
            bottomMargin: Theme.paddingMedium

            right: parent.right
            rightMargin: Theme.paddingMedium
        }

        fillMode: Image.PreserveAspectFit
        opacity: 0.15
    }

}
