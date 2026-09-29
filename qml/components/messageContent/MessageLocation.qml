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
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import "../"

MessageContentBase {
    id: contentItem
    height: width * 0.66666666;

    property var locationData : rawMessage.content.location
    property string fileExtra;

    // Only set while this device keeps the live location up to date
    readonly property real liveLocationEnd: rawMessage ? (liveLocationManager.activeLiveLocations[rawMessage.chat_id + ":" + rawMessage.id] || 0) : 0
    property real currentTime: Date.now()
    readonly property int liveLocationRemainingSeconds: liveLocationEnd > 0 ? Math.max(0, Math.ceil((liveLocationEnd - currentTime) / 1000)) : 0

    function formatRemainingTime(seconds) {
        var remainingSeconds = seconds % 60;
        return Math.floor(seconds / 60) + ":" + (remainingSeconds < 10 ? "0" : "") + remainingSeconds;
    }

    onClicked: {
        Qt.openUrlExternally("geo:" + locationData.latitude + "," + locationData.longitude);
    }
    onLocationDataChanged: updatePicture()
    onWidthChanged: updatePicture()

    function updatePicture() {
        if (locationData) {
            fileExtra = "location:" + locationData.latitude + ":" + locationData.longitude + ":" + Math.round(contentItem.width) + ":" + Math.round(contentItem.height);
            tdLibWrapper.getMapThumbnailFile(rawMessage.chat_id, locationData.latitude, locationData.longitude, Math.round(contentItem.width), Math.round(contentItem.height), fileExtra);
        }
    }

    Connections {
        target: tdLibWrapper
        onFileUpdated: {
            if(fileInformation["@extra"] === contentItem.fileExtra) {
                if(fileInformation.id !== image.file.fileId) {
                    image.fileInformation = fileInformation
                }
            }
        }
    }

    AppNotification {
        id: imageNotification
    }
    TDLibImage {
        id: image
        anchors.fill: parent
        cache: false
        highlighted: contentItem.highlighted
        Item {
            anchors.centerIn: parent
            width: markerImage.width
            height: markerImage.height * 1.75 // 0.875 (vertical pin point) * 2
            Icon {
                id: markerImage
                source: 'image://theme/icon-m-location'
            }

            DropShadow {
                anchors.fill: markerImage
                horizontalOffset: 3
                verticalOffset: 3
                radius: 8.0
                samples: 17
                color: Theme.colorScheme ? Theme.lightPrimaryColor : Theme.darkPrimaryColor
                source: markerImage
            }
        }
    }

    BackgroundImage {
        visible: image.status !== Image.Ready
    }

    Timer {
        interval: 1000
        repeat: true
        triggeredOnStart: true
        running: contentItem.liveLocationEnd > 0 && Qt.application.active
        onTriggered: contentItem.currentTime = Date.now()
    }

    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: Math.max(liveLocationLabel.height + 2 * Theme.paddingSmall, stopLiveLocationButton.height)
        color: Theme.rgba(Theme.highlightDimmerColor, Theme.opacityOverlay)
        visible: contentItem.liveLocationRemainingSeconds > 0

        Label {
            id: liveLocationLabel
            anchors {
                left: parent.left
                leftMargin: Theme.paddingMedium
                right: stopLiveLocationButton.left
                verticalCenter: parent.verticalCenter
            }
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.primaryColor
            wrapMode: Text.Wrap
            text: qsTr("Live location sharing active for %1 mins").arg(contentItem.formatRemainingTime(contentItem.liveLocationRemainingSeconds))
        }

        IconButton {
            id: stopLiveLocationButton
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            width: Theme.itemSizeExtraSmall
            height: Theme.itemSizeExtraSmall
            icon.source: "image://theme/icon-m-clear"
            icon.sourceSize: Qt.size(Theme.iconSizeSmallPlus, Theme.iconSizeSmallPlus)
            onClicked: liveLocationManager.stopLiveLocation(rawMessage.chat_id, rawMessage.id)
        }
    }

    Component.onCompleted: {
        updatePicture();
    }
}
