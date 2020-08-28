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
import QtQuick 2.5
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0

Item {

    id: documentPreviewItem
    width: parent.width
    height: Theme.itemSizeLarge

    property variant documentData;

    Component.onCompleted: {
        updateDocument();
    }

    function updateDocument() {
        if (typeof documentData === "object") {
            if (documentData.document.local.is_downloading_completed) {
                downloadDocumentButton.visible = false;
                openDocumentButton.visible = true;
            } else {
                openDocumentButton.visible = false;
                downloadDocumentButton.visible = true;
            }
        }
    }

    Connections {
        target: tdLibWrapper
        onFileUpdated: {
            if (typeof documentData !== "undefined" && fileId === documentData.document.id) {
                if (fileInformation.local.is_downloading_completed) {
                    downloadBusyIndicator.running = false;
                    documentData.document = fileInformation;
                    downloadDocumentButton.visible = false;
                    openDocumentButton.visible = true;
                    tdLibWrapper.openFileOnDevice(documentData.document.local.path);
                }
            }
        }
    }

    Button {
        id: downloadDocumentButton
        preferredWidth: Theme.buttonWidthMedium
        anchors.centerIn: parent
        text: qsTr("Download Document")
        visible: false
        onClicked: {
            downloadDocumentButton.visible = false;
            downloadBusyIndicator.running = true;
            tdLibWrapper.downloadFile(documentData.document.id);
        }
    }

    BusyIndicator {
        id: downloadBusyIndicator
        running: false
        size: BusyIndicatorSize.Medium
        visible: running
        anchors.centerIn: parent
    }

    Button {
        id: openDocumentButton
        preferredWidth: Theme.buttonWidthMedium
        anchors.centerIn: parent
        text: qsTr("Open Document")
        visible: false
        onClicked: {
            tdLibWrapper.openFileOnDevice(documentData.document.local.path);
        }
    }

}
