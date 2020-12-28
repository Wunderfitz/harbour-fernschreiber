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
import "../js/functions.js" as Functions
import "../js/debug.js" as Debug


Item {
    id: audioMessageComponent

    property ListItem messageListItem
    property MessageOverlayFlickable overlayFlickable
    property var rawMessage: messageListItem ? messageListItem.myMessage : overlayFlickable.overlayMessage

    property var audioData: ( rawMessage.content['@type'] === "messageVoiceNote" ) ?  rawMessage.content.voice_note : ( ( rawMessage.content['@type'] === "messageAudio" ) ? rawMessage.content.audio : "");
    property string audioUrl;
    property int previewFileId;
    property int audioFileId;
    property bool onScreen: messageListItem ? messageListItem.page.status === PageStatus.Active : true
    property string audioType : "voiceNote";
    property bool highlighted;
    signal clicked();

    width: parent.width
    height: width / 2

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
                if (fileId === audioFileId) {
                    downloadingProgressBar.maximumValue = fileInformation.size;
                    downloadingProgressBar.value = fileInformation.local.downloaded_size;
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
        layer.enabled: audioMessageComponent.highlighted
        layer.effect: PressEffect { source: placeholderImage }
    }

    BackgroundImage {
        id: backgroundImage
        visible: placeholderImage.status !== Image.Ready
        layer.enabled: audioMessageComponent.highlighted
        layer.effect: PressEffect { source: backgroundImage }
    }

    Rectangle {
        id: placeholderBackground
        color: "black"
        opacity: 0.3
        height: parent.height
        width: parent.width
        visible: playButton.visible
    }
    Label {
        visible: !!(audioData.performer || audioData.title)
        color: placeholderBackground.visible ? "white" : Theme.secondaryHighlightColor
        wrapMode: Text.Wrap
        anchors {
            fill: placeholderBackground
            margins: Theme.paddingSmall
        }
        text: audioData.performer + (audioData.performer && audioData.title ? " - " : "") + audioData.title
        font.pixelSize: Theme.fontSizeTiny
    }

    Column {
        width: parent.width
        height: downloadingProgressBar.height + audioControlRow.height
        anchors.centerIn: parent
        Row {
            id: audioControlRow
            width: parent.width
            height: Theme.iconSizeLarge
            Item {
                height: Theme.iconSizeLarge
                width: downloadItem.visible ? parent.width / 2 : parent.width
                IconButton {
                    id: playButton
                    anchors.centerIn: parent
                    width: Theme.iconSizeLarge
                    height: Theme.iconSizeLarge
                    icon {
                        source: "image://theme/icon-l-play?white"
                        asynchronous: true
                    }
                    highlighted: audioMessageComponent.highlighted || down
                    visible: placeholderImage.status === Image.Ready ? true : false
                    onClicked: {
                        handlePlay();
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
            Item {
                id: downloadItem
                width: parent.width / 2
                height: Theme.iconSizeLarge
                visible: audioData[audioType].local.is_downloading_completed
                Rectangle {
                    color: Theme.primaryColor
                    opacity: Theme.opacityFaint
                    width: Theme.iconSizeLarge * 0.9
                    height: Theme.iconSizeLarge * 0.9
                    anchors.centerIn: parent
                    radius: width / 2
                }

                IconButton {
                    id: downloadButton
                    anchors.centerIn: parent
                    width: Theme.iconSizeLarge
                    height: Theme.iconSizeLarge
                    icon {
                        source: "image://theme/icon-m-cloud-download?white"
                        asynchronous: true
                    }
                    highlighted: audioMessageComponent.highlighted || down
                    onClicked: {
                        tdLibWrapper.copyFileToDownloads(audioData[audioType].local.path);
                    }
                }
            }
        }
        ProgressBar {
            id: downloadingProgressBar
            minimumValue: 0
            maximumValue: 100
            value: 0
            visible: audioDownloadBusyIndicator.visible
            width: parent.width
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
                    downloadItem.visible = false;
                }
            }

            Connections {
                target: audioMessageComponent
                onClicked: {
                    if (messageAudio.playbackState === MediaPlayer.PlayingState) {
                        messageAudio.pause();
                        timeLeftItem.visible = true;
                    } else {
                        messageAudio.play();
                    }
                }
            }

            Audio {
                id: messageAudio

                Component.onCompleted: {
                    if (messageAudio.error === MediaPlayer.NoError) {
                        messageAudio.play();
                    } else {
                        errorText.text = qsTr("Error loading audio! " + messageAudio.errorString)
                        errorTextOverlay.visible = true;
                        errorText.visible = true;
                    }
                }

                onStatusChanged: {
                    if (status == MediaPlayer.NoMedia) {
                        Debug.log("No Media");
                        audioBusyIndicator.visible = false;
                    }
                    if (status == MediaPlayer.Loading) {
                        Debug.log("Loading");
                        audioBusyIndicator.visible = true;
                    }
                    if (status == MediaPlayer.Loaded) {
                        Debug.log("Loaded");
                        audioBusyIndicator.visible = false;
                    }
                    if (status == MediaPlayer.Buffering) {
                        Debug.log("Buffering");
                        audioBusyIndicator.visible = true;
                    }
                    if (status == MediaPlayer.Stalled) {
                        Debug.log("Stalled");
                        audioBusyIndicator.visible = true;
                    }
                    if (status == MediaPlayer.Buffered) {
                        Debug.log("Buffered");
                        audioBusyIndicator.visible = false;
                    }
                    if (status == MediaPlayer.EndOfMedia) {
                        Debug.log("End of Media");
                        audioBusyIndicator.visible = false;
                    }
                    if (status == MediaPlayer.InvalidMedia) {
                        Debug.log("Invalid Media");
                        audioBusyIndicator.visible = false;
                    }
                    if (status == MediaPlayer.UnknownStatus) {
                        Debug.log("Unknown Status");
                        audioBusyIndicator.visible = false;
                    }
                }

                source: audioUrl

                onStopped: {
                    playButton.visible = true;
                    downloadItem.visible = true;
                    audioComponentLoader.active = false;
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
                        width: parent.width / 2
                        IconButton {
                            id: pausedPlayButton
                            anchors.centerIn: parent
                            width: Theme.iconSizeLarge
                            height: Theme.iconSizeLarge
                            highlighted: audioMessageComponent.highlighted || down
                            icon {
                                asynchronous: true
                                source: "image://theme/icon-l-play?white"
                            }
                            onClicked: {
                                messageAudio.play();
                            }
                        }
                    }
                    Item {
                        id: pausedDownloadItem
                        width: parent.width / 2
                        height: parent.height
                        Rectangle {
                            color: Theme.primaryColor
                            opacity: Theme.opacityFaint
                            width: Theme.iconSizeLarge * 0.9
                            height: Theme.iconSizeLarge * 0.9
                            anchors.centerIn: parent
                            radius: width / 2
                        }

                        IconButton {
                            id: pausedDownloadButton
                            anchors.centerIn: parent
                            width: Theme.iconSizeLarge
                            height: Theme.iconSizeLarge
                            icon {
                                source: "image://theme/icon-m-cloud-download?white"
                                asynchronous: true
                            }
                            highlighted: audioMessageComponent.highlighted || down
                            onClicked: {
                                tdLibWrapper.copyFileToDownloads(audioData[audioType].local.path);
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
                    highlighted: audioMessageComponent.highlighted || down
                    onReleased: {
                        messageAudio.seek(Math.floor(value));
                        messageAudio.play();
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
