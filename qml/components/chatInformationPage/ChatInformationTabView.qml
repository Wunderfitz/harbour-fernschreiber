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

import "./"
import "../"
import "../../pages"

Item {
    id: tabViewItem
    property alias count: tabView.count
    height: Screen.width
    opacity: count > 0 ? 1.0 : 0.0
    Behavior on height { PropertyAnimation {duration: 300}}
    Behavior on opacity { PropertyAnimation {duration: 300}}

    Item {
        id: tabViewHeader
        /*
         * Tab view is prepared, but "disabled" for now
         * to view shared media/links/…,
         * we need message search with filters
         */

        // START: change this to enable
        height: visible ? headerGrid.height : 0
//        height: 0
        clip: true
        visible: tabView.count > 1
        // END: change this to enable

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        Grid {
            id: headerGrid
            width: parent.width
            columns: tabView.count
            Repeater {
                model: tabView.count
                delegate: BackgroundItem {
                    id: headerItem
                    property bool loaded: tabItem.image !== "" && tabItem.title !== ""
                    width: loaded ? (headerGrid.width / tabView.count) : 0
                    opacity: loaded ? 1.0 : 0.0

                    Behavior on width { PropertyAnimation {duration: 300}}
                    Behavior on opacity { PropertyAnimation {duration: 300}}
                    height: Theme.itemSizeLarge
                    property ChatInformationTabItemBase tabItem: tabView.model.get(index)
                    property int itemIndex: index
                    property bool itemIsActive: tabItem.active
                    Icon {
                        id: headerIcon
                        source: headerItem.tabItem.image
                        highlighted: headerItem.pressed || headerItem.itemIsActive
                        anchors {
                            top: parent.top
                            horizontalCenter: parent.horizontalCenter
                        }
                    }
                    Label {
                        text: headerItem.tabItem.title// + headerItem.itemIsActive
                        width: parent.width
                        horizontalAlignment: Text.AlignHCenter
                        anchors.top: headerIcon.bottom
                        highlighted: headerItem.pressed || headerItem.itemIsActive
                        font.pixelSize: Theme.fontSizeTiny
                    }
                    onClicked: {
                        chatInformationPage.scrollDown()
                        tabView.openTab(itemIndex)
                    }
                }
            }
        }
    }

    ListView {
        id: tabView
        orientation: ListView.Horizontal
        clip: true
        snapMode: ListView.SnapOneItem
        highlightRangeMode: ListView.StrictlyEnforceRange
        highlightFollowsCurrentItem: true
        boundsBehavior: Flickable.StopAtBounds
        currentIndex: 0
        highlightMoveDuration: 500
        property int maxHeight: tabViewItem.height - tabViewHeader.height


        anchors {
            top: tabViewHeader.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        function openTab(index) {
            currentIndex = index;
        }
        model: VisualItemModel {
            id: tabModel
        }
    }
    Component {
        id: membersGroupComponent
        ChatInformationTabItemMembersGroups{
            width: tabView.width
        }
    }
    Component {
        id: settingsComponent
        ChatInformationTabItemSettings {
            width: tabView.width
        }
    }
    Component {
        id: debugComponent
        ChatInformationTabItemDebug {
            width: tabView.width
        }
    }
    property var tabItems: {
        var items = [];
        if(!(isPrivateChat && chatPartnerGroupId === myUserId.toString())) {
            items.push(membersGroupComponent);
        }
        if(!isPrivateChat && (groupInformation.status.can_restrict_members || groupInformation.status["@type"] === "chatMemberStatusCreator")) {
            items.push(settingsComponent);
        }
//        items.push(debugComponent);

        return items;
    }
    onTabItemsChanged: fillTabItems()

    function fillTabItems() {
        tabModel.clear()
        for(var i in tabItems) {
            tabModel.append(tabItems[i].createObject(tabModel));
        }
    }

    Component.onCompleted: {
        fillTabItems()
    }

}
