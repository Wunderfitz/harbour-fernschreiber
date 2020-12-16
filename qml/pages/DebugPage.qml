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
import WerkWolf.Fernschreiber 1.0
import "../components"
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions
import "../js/debug.js" as Debug

Page {
    id: debugPage
    allowedOrientations: Orientation.All

    SilicaFlickable {
        id: aboutContainer
        contentHeight: column.height
        anchors.fill: parent

        Column {
            id: column
            width: parent.width
            spacing: Theme.paddingLarge
            PageHeader {
                title: "Debug Page"
                description: "here be dragons"

            }

            SectionHeader {
                text: "Join chat by id"
            }

            Row {
                x: Theme.horizontalPageMargin
                TextField {
                    id: chatId
                    anchors.bottom: parent.bottom
                    width: column.width - joinButton.width - Theme.horizontalPageMargin - Theme.paddingLarge
                    placeholderText: "Chat id"
                    labelVisible: false
                    EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                    EnterKey.enabled: text.length > 0
                    EnterKey.onClicked: tdLibWrapper.joinChat(text)
                }
                Button {
                    id: joinButton
                    text: "Join"
                    anchors.bottom: parent.bottom
                    enabled: chatId.text.length > 0
                    onClicked: tdLibWrapper.joinChat(chatId.text)
                }
            }
        }

        VerticalScrollDecorator {}
    }

    Timer {
        id: profileTimer
        interval: 1000
        property bool hasRun
        property var cases: []
        onTriggered: {
            if(cases.length === 0) {
                return;
            }

            if(!hasRun) {
                hasRun = true;
                Debug.profile();
            }
            cases.pop()();

            if(cases.length > 0) {
                restart();
            } else {
                Debug.profileEnd();
            }
        }
    }

    onStatusChanged: {
        if (status === PageStatus.Active) {
            // example runner for comparing function calls

//            profileTimer.cases.push(function(){
//                Debug.compareAndRepeat(
//                            "getUserName",
//                            Functions.getUserName,
//                            Functions.getUserName,
//                            [
//                                [{first_name: "Test", last_name: "User"}],
//                                [{first_name: "Test", last_name: ""}],
//                                [{first_name: "Test"}],
//                                [{first_name: "", last_name: "User"}],
//                                [{last_name: "User"}]
//                            ],
//                            800
//                            )
//            });
//            profileTimer.start();
        }
    }
}
