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
