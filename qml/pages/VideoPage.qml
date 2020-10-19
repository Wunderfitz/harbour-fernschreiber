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
import QtQuick 2.0
import Sailfish.Silica 1.0
import QtMultimedia 5.0
import "../components"
import "../js/functions.js" as Functions

Page {
    id: videoPage
    allowedOrientations: Orientation.All

    property variant videoData;

    property int videoWidth : videoData.width
    property int videoHeight : videoData.height
    property string videoUrl;

    property real imageSizeFactor : videoWidth / videoHeight;
    property real screenSizeFactor: videoPage.width / videoPage.height;
    property real sizingFactor    : imageSizeFactor >= screenSizeFactor ? videoPage.width / videoWidth : videoPage.height / videoHeight;

    Component.onCompleted: {
        updateVideoData();
    }

    function updateVideoData() {
        if (typeof videoData === "object") {
            if (videoData.video.local.is_downloading_completed) {
                videoPage.videoUrl = videoData.video.local.path;
            }
        }
    }

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            id: videoPagePullDownMenu
            visible: (videoPage.videoUrl !== "")
            MenuItem {
                text: qsTr("Download Video")
                onClicked: {
                    tdLibWrapper.copyFileToDownloads(videoPage.videoUrl);
                }
            }
        }

        Connections {
            target: tdLibWrapper
            onFileUpdated: {
                if (fileId === videoPage.video.id) {
                    if (fileInformation.local.is_downloading_completed) {
                        videoPage.video = fileInformation;
                        videoPage.videoUrl = fileInformation.local.path;
                        videoPagePullDownMenu.visible = true;
                    }
                }
            }
            onCopyToDownloadsSuccessful: {
                videoNotification.show(qsTr("Download of %1 successful.").arg(fileName), filePath);
            }

            onCopyToDownloadsError: {
                videoNotification.show(qsTr("Download failed."));
            }
        }

        AppNotification {
            id: videoNotification
        }

        Item {
            width: videoPage.videoWidth * videoPage.sizingFactor
            height: videoPage.videoHeight * videoPage.sizingFactor
            anchors.centerIn: parent

            VideoPreview {
                videoData: videoPage.videoData
                fullscreen: true
                onScreen: videoPage.status === PageStatus.Active
            }
        }

    }
}

