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

Column {
    id: editAreaColumn
    visible: canEdit || text !== ""
    property bool canEdit
    property alias headerText: editAreaHeader.text
    property string emptyPlaceholderText
    property string text
    property bool multiLine
    property bool headerLeftAligned

    property bool isEditing
    property Item editItem: multiLine ? editAreaTextArea : editAreaTextField

    signal saveButtonClicked(string textValue)
    signal editButtonClicked()
    signal textEdited(string textValue)
    width: parent.width


    SectionHeader {
        id: editAreaHeader
        height: parent.visible && text !== "" ? Theme.itemSizeExtraSmall : 0
        x: 0
        horizontalAlignment: headerLeftAligned ? Text.AlignLeft : Text.AlignRight
    }
    Row {
        id: editAreaTextRow
        width: parent.width

        TextArea {
            id: editAreaTextArea
            visible: editAreaColumn.isEditing && editAreaColumn.multiLine
            width: parent.width - editAreaButton.width
            textLeftMargin: 0
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Theme.fontSizeSmall
        }
        TextField {
            id: editAreaTextField
            visible: editAreaColumn.isEditing && !editAreaColumn.multiLine
            width: parent.width - editAreaButton.width
            anchors.verticalCenter: parent.verticalCenter
            textLeftMargin: 0
            EnterKey.onClicked: {
                editAreaColumn.isEditing = false;
                editAreaColumn.saveButtonClicked(editAreaColumn.editItem.text);
            }
            EnterKey.iconSource: editAreaButton.icon.source
            font.pixelSize: Theme.fontSizeSmall
        }
        InformationTextItem {
            id: editAreaTextItem
            visible: !editAreaColumn.isEditing
            anchors.verticalCenter: parent.verticalCenter
            text: editAreaColumn.text || editAreaColumn.emptyPlaceholderText
            width: parent.width - editAreaButton.width
            height: !editAreaColumn.multiLine ? implicitHeight : editAreaTextField.height
        }
        IconButton {
            id: editAreaButton
            visible: editAreaColumn.canEdit
            width: visible ? Theme.itemSizeSmall : 0
            icon.source: editAreaColumn.isEditing ? "image://theme/icon-m-accept" : "image://theme/icon-m-edit"
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                if(editAreaColumn.isEditing) {
                    editAreaColumn.isEditing = false;
                    editAreaColumn.saveButtonClicked(editAreaColumn.editItem.text);
                } else {
                    editAreaColumn.isEditing = true;
                    editAreaColumn.editItem.text = editAreaColumn.text;
                    editAreaColumn.editItem.forceActiveFocus();
                    editAreaColumn.editItem.cursorPosition = editAreaColumn.editItem.text.length;
                    editAreaColumn.editButtonClicked();
                }
            }
        }
    }
    Connections {
        target: editItem
        onTextChanged: {
            if(editItem.focus) {
                editAreaColumn.textEdited(editItem.text)
            }
        }
    }
}
