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
import QtMultimedia 5.6
import "../components"
import "../js/functions.js" as Functions
import "../components/messageContent"

Page {
    id: videoPage
    allowedOrientations: Orientation.All
    property alias isVideoNote: messageVideo.isVideoNote;
    property alias contentBase: messageVideo.contentBase;
    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            id: videoPagePullDownMenu
            visible: messageVideo.file.isDownloadingCompleted
            MenuItem {
                text: qsTr("Download Video")
                onClicked: {
                    tdLibWrapper.copyFileToDownloads(messageVideo.file.path);
                }
            }
        }

        Connections {
            target: tdLibWrapper
            onCopyToDownloadsSuccessful: {
                appNotification.show(qsTr("Download of %1 successful.").arg(fileName), filePath);
            }
            onCopyToDownloadsError: {
                appNotification.show(qsTr("Download failed."));
            }
        }

        MessageVideo {
            id: messageVideo
            anchors.fill: parent
            fullscreen: true
            playRequested: true
            onScreen: videoPage.status === PageStatus.Active
        }

    }
}

