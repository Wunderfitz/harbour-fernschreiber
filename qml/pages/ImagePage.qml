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
import "../js/functions.js" as Functions
import "../js/debug.js" as Debug

Page {
    id: imagePage
    allowedOrientations: Orientation.All
    backNavigation: !imageOnly

    property var photoData;
    property var pictureFileInformation;

    property string imageUrl;
    property int imageWidth;
    property int imageHeight;

    property real imageSizeFactor : imageWidth / imageHeight;
    property real screenSizeFactor: imagePage.width / imagePage.height;
    property real sizingFactor    : imageSizeFactor >= screenSizeFactor ? imagePage.width / imageWidth : imagePage.height / imageHeight;

    property real previousScale : 1;
    property real centerX;
    property real centerY;
    property real oldCenterX;
    property real oldCenterY;

    property bool imageOnly

    Component.onCompleted: {
        updatePicture();
    }

    function updatePicture() {
        if (typeof photoData === "object") {
            // Check first which size fits best...
            for (var i = 0; i < photoData.sizes.length; i++) {
                imagePage.imageWidth = photoData.sizes[i].width;
                imagePage.imageHeight = photoData.sizes[i].height;
                imagePage.pictureFileInformation = photoData.sizes[i].photo;
                if (photoData.sizes[i].width >= imagePage.width) {
                    break;
                }
            }

            if (imagePage.pictureFileInformation.local.is_downloading_completed) {
                imagePage.imageUrl = imagePage.pictureFileInformation.local.path;
            } else {
                tdLibWrapper.downloadFile(imagePage.pictureFileInformation.id);
            }
        }
    }

    Connections {
        target: tdLibWrapper
        onFileUpdated: {
            if (fileId === imagePage.pictureFileInformation.id) {
                Debug.log("File updated, completed? ", fileInformation.local.is_downloading_completed);
                if (fileInformation.local.is_downloading_completed) {
                    imagePage.pictureFileInformation = fileInformation;
                    imagePage.imageUrl = fileInformation.local.path;
                }
            }
        }
        onCopyToDownloadsSuccessful: {
            imageNotification.show(qsTr("Download of %1 successful.").arg(fileName), filePath);
        }

        onCopyToDownloadsError: {
            imageNotification.show(qsTr("Download failed."));
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        interactive: !imageOnly

        PullDownMenu {
            visible: !imageOnly && imageUrl
            MenuItem {
                text: qsTr("Download Picture")
                onClicked: {
                    tdLibWrapper.copyFileToDownloads(imagePage.imageUrl);
                }
            }
        }

        AppNotification {
            id: imageNotification
        }

        SilicaFlickable {
            id: imageFlickable
            anchors.fill: parent
            clip: true
            contentWidth: imagePinchArea.width
            contentHeight: imagePinchArea.height
            flickableDirection: Flickable.HorizontalAndVerticalFlick
            quickScrollEnabled: false

            PinchArea {
                id: imagePinchArea
                width:  Math.max( singleImage.width * singleImage.scale, imageFlickable.width )
                height: Math.max( singleImage.height * singleImage.scale, imageFlickable.height )

                enabled: singleImage.status === Image.Ready
                pinch {
                    target: singleImage
                    minimumScale: 1
                    maximumScale: 4
                }

                onPinchUpdated: {
                    imagePage.centerX = pinch.center.x
                    imagePage.centerY = pinch.center.y
                    imageFlickable.returnToBounds()
                }

                Image {
                    id: singleImage
                    source: imageUrl
                    width: imagePage.imageWidth * imagePage.sizingFactor
                    height: imagePage.imageHeight * imagePage.sizingFactor
                    anchors.centerIn: parent

                    fillMode: Image.PreserveAspectFit
                    asynchronous: true

                    visible: opacity > 0
                    opacity: status === Image.Ready ? 1 : 0
                    Behavior on opacity { FadeAnimation {} }
                    onScaleChanged: {
                        var newWidth = singleImage.width * singleImage.scale;
                        var newHeight = singleImage.height * singleImage.scale;
                        var oldWidth = singleImage.width * imagePage.previousScale;
                        var oldHeight = singleImage.height * imagePage.previousScale;
                        var widthDifference = newWidth - oldWidth;
                        var heightDifference = newHeight - oldHeight;

                        if (oldWidth > imageFlickable.width || newWidth > imageFlickable.width) {
                            var xRatioNew = imagePage.centerX / newWidth;
                            var xRatioOld = imagePage.centerX / oldHeight;
                            imageFlickable.contentX = imageFlickable.contentX + ( xRatioNew * widthDifference );
                        }
                        if (oldHeight > imageFlickable.height || newHeight > imageFlickable.height) {
                            var yRatioNew = imagePage.centerY / newHeight;
                            var yRatioOld = imagePage.centerY / oldHeight;
                            imageFlickable.contentY = imageFlickable.contentY + ( yRatioNew * heightDifference );
                        }

                        imagePage.previousScale = singleImage.scale;
                        imagePage.oldCenterX = imagePage.centerX;
                        imagePage.oldCenterY = imagePage.centerY;
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    visible: singleImage.visible

                    onClicked: imageOnly = !imageOnly // Toggle "Image only" mode on tap
                    onDoubleClicked: ; // This introduces a delay before onClicked is invoked
                }
            }

            ScrollDecorator { flickable: imageFlickable }
        }
    }

    BackgroundImage {
        visible: singleImage.status !== Image.Ready
    }
}
