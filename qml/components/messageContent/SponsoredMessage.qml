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
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import WerkWolf.Fernschreiber 1.0
import "../"
import "../../js/functions.js" as Functions

Column {
    id: sponsoredMessageColumn

    property var sponsoredMessageData;

    // A sponsored message advertises the URL of its sponsor. That can be an
    // ordinary web address or a Telegram link into a chat, a message or a bot -
    // Functions.handleLink() knows how to tell those apart. Before TDLib 1.8.50
    // the sponsor was described by an internal link instead.
    readonly property string sponsorUrl: (sponsoredMessageData && sponsoredMessageData.sponsor) ? (sponsoredMessageData.sponsor.url || "") : ""

    Button {
        id: sponsoredMessageButton
        visible: sponsoredMessageColumn.sponsorUrl !== ""
        text: (sponsoredMessageData && sponsoredMessageData.button_text) ? sponsoredMessageData.button_text : qsTr("Go to Channel")
        anchors {
            horizontalCenter: parent.horizontalCenter
        }
        onClicked: {
            Functions.handleLink(sponsoredMessageColumn.sponsorUrl);
        }
    }

}
