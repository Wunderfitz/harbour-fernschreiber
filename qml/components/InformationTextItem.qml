/*
    Copyright (C) 2020-21 Sebastian J. Wolf and other contributors

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

import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions

Column {
    id: textItem
    property alias headerText: headerItem.text
    property string text
    property bool isLinkedLabel // for telephone number
    width: parent.width
    visible: !!text
    SectionHeader {
        id: headerItem
        visible: text !== "" && labelLoader.status === Loader.Ready && labelLoader.item.text !== ""
        height: visible ? Theme.itemSizeSmall : 0
        x: 0
    }
    Loader {
        id: labelLoader
        active: true
        asynchronous: true
        sourceComponent: textItem.isLinkedLabel ? linkedLabelComponent : labelComponent
        width: textItem.width
    }

    Component {
        id: labelComponent
        Label {
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            font.pixelSize: Theme.fontSizeSmall
            textFormat: Text.StyledText
            color: Theme.primaryColor
            text: Emoji.emojify( Functions.replaceUrlsWithLinks(textItem.text).replace(/\n/g, "<br>"), Theme.fontSizeExtraSmall)
            linkColor: Theme.highlightColor
            visible: text !== ""
            onLinkActivated: {
                Functions.handleLink(link);
            }
        }
    }
    Component {
        id: linkedLabelComponent
        LinkedLabel {
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            font.pixelSize: Theme.fontSizeSmall
            textFormat: Text.StyledText
            color: Theme.highlightColor
            plainText: textItem.text
            visible: textItem.text !== ""
            onLinkActivated: {
                Functions.handleLink(link);
            }
        }
    }
}
