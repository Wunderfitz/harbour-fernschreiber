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
import QtQuick 2.0
import Sailfish.Silica 1.0
import QtMultimedia 5.0
import "../js/functions.js" as Functions

Item {
    id: audioMessageComponent

    property variant audioData;
    property string audioUrl;
    property int previewFileId;
    property int audioFileId;
    property bool onScreen;
    property string audioType : "voiceNote";

    width: parent.width
    height: parent.height

    function getTimeString(rawSeconds) {
        var minutes = Math.floor( rawSeconds / 60 );
        var seconds = rawSeconds - ( minutes * 60 );

        if ( minutes < 10 ) {
            minutes = "0" + minutes;
        }
        if ( seconds < 10 ) {
            seconds = "0" + seconds;
        }
        return minutes + ":" + seconds;
    }

    Component.onCompleted: {
        updateAudioThumbnail();
    }

    function updateAudioThumbnail() {
        if (audioData) {
            audioType = ( audioData['@type'] === "voiceNote" ) ? "voice" : "audio";
            audioFileId = audioData[audioType].id;
            if (typeof audioData.album_cover_thumbnail !== "undefined") {
                previewFileId = audioData.album_cover_thumbnail.photo.id;
                if (audioData.album_cover_thumbnail.photo.local.is_downloading_completed) {
                    placeholderImage.source = audioData.album_cover_thumbnail.photo.local.path;
                } else {
                    tdLibWrapper.downloadFile(previewFileId);
                }
            } else {
                placeholderImage.source = "image://theme/icon-l-music?white";
                placeholderImage.width = Theme.itemSizeLarge
                placeholderImage.height = Theme.itemSizeLarge
            }
        }
    }

    function handlePlay() {
        if (audioData[audioType].local.is_downloading_completed) {
            audioUrl = audioData[audioType].local.path;
            audioComponentLoader.active = true;
        } else {
            audioDownloadBusyIndicator.running = true;
            tdLibWrapper.downloadFile(audioFileId);
        }
    }

    Connections {
        target: tdLibWrapper
        onFileUpdated: {
            if (typeof audioData === "object") {
                if (fileInformation.local.is_downloading_completed) {
                    if (fileId === previewFileId) {
                        audioData.thumbnail.photo = fileInformation;
                        placeholderImage.source = fileInformation.local.path;
                    }
                    if (fileId === audioFileId) {
                        audioDownloadBusyIndicator.running = false;
                        audioData[audioType] = fileInformation;
                        audioUrl = fileInformation.local.path;
                        if (onScreen) {
                            audioComponentLoader.active = true;
                        }
                    }
                }
            }
        }
    }

    Image {
        id: placeholderImage
        width: parent.width
        height: parent.height
        anchors.centerIn: parent
        asynchronous: true
        fillMode: Image.PreserveAspectCrop
        visible: status === Image.Ready ? true : false
    }

    Image {
        id: imageLoadingBackgroundImage
        source: "../../images/background-" + ( Theme.colorScheme ? "black" : "white" ) + "-small.png"
        anchors {
            centerIn: parent
        }
        width: parent.width - Theme.paddingSmall
        height: parent.height - Theme.paddingSmall
        visible: placeholderImage.status !== Image.Ready
        asynchronous: true

        fillMode: Image.PreserveAspectFit
        opacity: 0.15
    }

    Rectangle {
        id: placeholderBackground
        color: "black"
        opacity: 0.3
        height: parent.height
        width: parent.width
        visible: playButton.visible
    }

    Row {
        width: parent.width
        height: parent.height
        Item {
            height: parent.height
            width: parent.width
            Image {
                id: playButton
                anchors.centerIn: parent
                width: Theme.iconSizeLarge
                height: Theme.iconSizeLarge
                source: "image://theme/icon-l-play?white"
                asynchronous: true
                visible: placeholderImage.status === Image.Ready ? true : false
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        handlePlay();
                    }
                }
            }
            BusyIndicator {
                id: audioDownloadBusyIndicator
                running: false
                visible: running
                anchors.centerIn: parent
                size: BusyIndicatorSize.Large
            }
        }
    }

    Rectangle {
        id: audioErrorShade
        width: parent.width
        height: parent.height
        color: "lightgrey"
        visible: placeholderImage.status === Image.Error ? true : false
        opacity: 0.3
    }

    Rectangle {
        id: errorTextOverlay
        color: "black"
        opacity: 0.8
        width: parent.width
        height: parent.height
        visible: false
    }

    Text {
        id: errorText
        visible: false
        width: parent.width
        color: Theme.primaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
        horizontalAlignment: Text.AlignHCenter
        anchors {
            verticalCenter: parent.verticalCenter
        }
        wrapMode: Text.Wrap
        text: ""
    }

    Loader {
        id: audioComponentLoader
        active: false
        width: parent.width
        height: parent.height
        sourceComponent: audioComponent
    }

    Component {
        id: audioComponent

        Item {
            width: parent ? parent.width : 0
            height: parent ? parent.height : 0

            Connections {
                target: messageAudio
                onPlaying: {
                    playButton.visible = false;
                }
            }

            Audio {
                id: messageAudio

                Component.onCompleted: {
                    if (messageAudio.error === MediaPlayer.NoError) {
                        messageAudio.play();
                        timeLeftTimer.start();
                    } else {
                        errorText.text = qsTr("Error loading audio! " + messageAudio.errorString)
                        errorTextOverlay.visible = true;
                        errorText.visible = true;
                    }
                }

                onStatusChanged: {
                    if (status == MediaPlayer.NoMedia) {
                        console.log("No Media");
                        audioBusyIndicator.visible = false;
                    }
                    if (status == MediaPlayer.Loading) {
                        console.log("Loading");
                        audioBusyIndicator.visible = true;
                    }
                    if (status == MediaPlayer.Loaded) {
                        console.log("Loaded");
                        audioBusyIndicator.visible = false;
                    }
                    if (status == MediaPlayer.Buffering) {
                        console.log("Buffering");
                        audioBusyIndicator.visible = true;
                    }
                    if (status == MediaPlayer.Stalled) {
                        console.log("Stalled");
                        audioBusyIndicator.visible = true;
                    }
                    if (status == MediaPlayer.Buffered) {
                        console.log("Buffered");
                        audioBusyIndicator.visible = false;
                    }
                    if (status == MediaPlayer.EndOfMedia) {
                        console.log("End of Media");
                        audioBusyIndicator.visible = false;
                    }
                    if (status == MediaPlayer.InvalidMedia) {
                        console.log("Invalid Media");
                        audioBusyIndicator.visible = false;
                    }
                    if (status == MediaPlayer.UnknownStatus) {
                        console.log("Unknown Status");
                        audioBusyIndicator.visible = false;
                    }
                }

                source: audioUrl

                onStopped: {
                    playButton.visible = true;
                    audioComponentLoader.active = false;
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (messageAudio.playbackState === MediaPlayer.PlayingState) {
                        messageAudio.pause();
                        timeLeftItem.visible = true;
                    } else {
                        messageAudio.play();
                        timeLeftTimer.start();
                    }
                }
            }

            BusyIndicator {
                id: audioBusyIndicator
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                visible: false
                running: visible
                size: BusyIndicatorSize.Medium
            }

            Timer {
                id: timeLeftTimer
                repeat: false
                interval: 2000
                onTriggered: {
                    timeLeftItem.visible = false;
                }
            }

            Item {
                id: timeLeftItem
                width: parent.width
                height: parent.height
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                opacity: visible ? 1 : 0
                Behavior on opacity { NumberAnimation {} }

                Rectangle {
                    id: positionTextOverlay
                    color: "black"
                    opacity: 0.3
                    width: parent.width
                    height: parent.height
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: pausedRow.visible
                }

                Row {
                    id: pausedRow
                    width: parent.width
                    height: parent.height - ( messageAudioSlider.visible ? messageAudioSlider.height : 0 ) - ( positionText.visible ? positionText.height : 0 )
                    visible: audioComponentLoader.active && messageAudio.playbackState === MediaPlayer.PausedState
                    Item {
                        height: parent.height
                        width: parent.width
                        Image {
                            id: pausedPlayButton
                            anchors.centerIn: parent
                            width: Theme.iconSizeLarge
                            height: Theme.iconSizeLarge
                            asynchronous: true
                            source: "image://theme/icon-l-play?white"
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    messageAudio.play();
                                    timeLeftTimer.start();
                                }
                            }
                        }
                    }
                }

                Slider {
                    id: messageAudioSlider
                    width: parent.width
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: positionText.top
                    minimumValue: 0
                    maximumValue: messageAudio.duration ? messageAudio.duration : 0
                    stepSize: 1
                    value: messageAudio.position
                    enabled: messageAudio.seekable
                    visible: (messageAudio.duration > 0)
                    onReleased: {
                        messageAudio.seek(Math.floor(value));
                        messageAudio.play();
                        timeLeftTimer.start();
                    }
                    valueText: getTimeString(Math.round((messageAudio.duration - messageAudioSlider.value) / 1000))
                }

                Text {
                    id: positionText
                    visible: messageAudio.duration === 0
                    color: Theme.primaryColor
                    font.pixelSize: Theme.fontSizeTiny
                    anchors {
                        bottom: parent.bottom
                        bottomMargin: Theme.paddingSmall
                        horizontalCenter: positionTextOverlay.horizontalCenter
                    }
                    wrapMode: Text.Wrap
                    text: ( messageAudio.duration - messageAudio.position ) > 0 ? getTimeString(Math.round((messageAudio.duration - messageAudio.position) / 1000)) : "-:-"
                }
            }

        }


    }

}
