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
import QtQuick 2.6
import Sailfish.Silica 1.0
import "../../pages"
import "../"

Item {
    id: tabItem
    property string title
    property url image
    property bool loading
    //overrideable:
    property alias loadingVisible: loadingColumn.loadingVisible
    property string loadingText

    property int tabIndex: tabItem.VisualItemModel.index
    property bool active: tabItem.ListView.isCurrentItem

    default property alias _data: contentItem.data


    width: parent.width
    height: tabView.maxHeight//Math.max(contentItem.height, loadingColumn.height)
    opacity: active ? 1.0 : 0.0
    Behavior on opacity { PropertyAnimation {duration: 300}}

    Column {
        id: loadingColumn
        property bool loadingVisible: tabItem.loading
        width: tabItem.width
        height: loadingLabel.height + loadingBusyIndicator.height + Theme.paddingMedium
        spacing: Theme.paddingMedium
        topPadding: Theme.paddingLarge
        anchors.top: parent.top
        opacity: loadingVisible ? 1.0 : 0.0
        Behavior on opacity { NumberAnimation {} }
        visible: tabItem.loading

        InfoLabel {
            id: loadingLabel
            text: tabItem.loadingText
        }

        BusyIndicator {
            id: loadingBusyIndicator
            anchors.horizontalCenter: parent.horizontalCenter
            running: parent.loadingVisible
            size: BusyIndicatorSize.Large
        }
    }
    Item {
        id: contentItem
        width: parent.width
        height: childrenRect.height
    }

}
