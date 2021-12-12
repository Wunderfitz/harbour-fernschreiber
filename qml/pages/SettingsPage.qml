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
import Sailfish.Pickers 1.0
import WerkWolf.Fernschreiber 1.0
import "../components"
import "../components/settingsPage"
import "../js/functions.js" as Functions
import "../js/debug.js" as Debug

Page {
    id: settingsPage
    allowedOrientations: Orientation.All

    SilicaFlickable {
        id: settingsContainer
        contentHeight: column.height
        anchors.fill: parent

        Column {
            id: column
            width: settingsPage.width
            bottomPadding: Theme.paddingLarge

            PageHeader {
                title: qsTr("Settings")
            }

            Accordion {
                flickable: settingsContainer
                SettingsUserProfile { expanded: true; asynchronous: false }
                SettingsSession {}
                SettingsPrivacy {}
                SettingsBehavior {}
                SettingsAppearance {}
                SettingsStorage {}
            }
        }

        VerticalScrollDecorator {}
    }
}
