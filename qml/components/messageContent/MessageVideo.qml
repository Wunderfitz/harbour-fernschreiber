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
import WerkWolf.Fernschreiber 1.0
import QtMultimedia 5.6
import "../"
import "../../js/functions.js" as Functions
import "../../js/debug.js" as Debug

MessageContentBase {
    id: videoMessageComponent

    property bool isVideoNote : rawMessage.content['@type'] === "messageVideoNote";

    property var contentBase: ( rawMessage.content['@type'] === "messageVideo" ) ?  rawMessage.content.video : ( ( rawMessage.content['@type'] === "messageAnimation" ) ? rawMessage.content.animation : rawMessage.content.video_note )
    property bool fullscreen: false;
    property bool onScreen: messageListItem ? messageListItem.page.status === PageStatus.Active : true;
    property string videoType : "video";
    property bool playRequested: false;

    property alias file: file;

    height: videoMessageComponent.isVideoNote ? width : Functions.getVideoHeight(width, videoData)


    TDLibFile {
        id: file
        tdlib: tdLibWrapper
        fileInformation: videoMessageComponent.contentBase.animation || videoMessageComponent.contentBase.video
        autoLoad: playRequested
    }

    Timer {
        id: screensaverTimer
        interval: 30000
        running: false
        repeat: true
        triggeredOnStart: true
        onTriggered: {
            tdLibWrapper.controlScreenSaver(false);
        }
    }

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

    function disableScreensaver() {
        screensaverTimer.start();
    }

    function enableScreensaver() {
        screensaverTimer.stop();
        tdLibWrapper.controlScreenSaver(true);
    }

    Component.onCompleted: {
        if (typeof rawMessage !== "undefined" && rawMessage.content['@type'] === "messageAnimation") {
            handlePlay();
        }
    }

    function handlePlay() {
        if(file.isDownloadingCompleted) {
            messageVideo.play();
        } else {
            playRequested = true;
        }
    }

    TDLibThumbnail {
        id: thumbnail
        anchors.fill: parent
        thumbnail: contentBase.thumbnail
        minithumbnail: contentBase.minithumbnail
        highlighted: videoMessageComponent.highlighted
        visible: controls.visible
    }


    Video {
        id: messageVideo

        Component.onCompleted: {
            if (messageVideo.error !== MediaPlayer.NoError) {
                errorText.text = qsTr("Error loading video! " + messageVideo.errorString)
                errorTextOverlay.visible = true;
            }
        }

        onStatusChanged: {
            if (status == MediaPlayer.NoMedia) {
                Debug.log("No Media");
                videoBusyIndicator.visible = false;
            }
            if (status == MediaPlayer.Loading) {
                Debug.log("Loading");
                videoBusyIndicator.visible = true;
            }
            if (status == MediaPlayer.Loaded) {
                Debug.log("Loaded");
                videoBusyIndicator.visible = false;
            }
            if (status == MediaPlayer.Buffering) {
                Debug.log("Buffering");
                videoBusyIndicator.visible = true;
            }
            if (status == MediaPlayer.Stalled) {
                Debug.log("Stalled");
                videoBusyIndicator.visible = true;
            }
            if (status == MediaPlayer.Buffered) {
                Debug.log("Buffered");
                videoBusyIndicator.visible = false;
            }
            if (status == MediaPlayer.EndOfMedia) {
                Debug.log("End of Media");
                videoBusyIndicator.visible = false;
            }
            if (status == MediaPlayer.InvalidMedia) {
                Debug.log("Invalid Media");
                videoBusyIndicator.visible = false;
            }
            if (status == MediaPlayer.UnknownStatus) {
                Debug.log("Unknown Status");
                videoBusyIndicator.visible = false;
            }
        }

        //                visible: false
        width: parent.width
        height: parent.height
        source: file.path
        autoPlay: playRequested
        layer.enabled: videoMessageComponent.highlighted
        layer.effect: PressEffect { source: messageVideo }
        //        fillMode: VideoOutput.PreserveAspectFit
        onPlaying: {
            disableScreensaver();
            sliderTimer.start();
        }
        onPaused: {
            enableScreensaver();
        }

        onStopped: {
            enableScreensaver();
        }

        MouseArea {
            enabled: messageVideo.playbackState === MediaPlayer.PlayingState
            anchors.fill: parent
            onClicked: {
                messageVideo.pause();
            }
        }
    }

    Rectangle {
        color: "black"
        opacity: controls.visible ? 0.3 : 0.0
        anchors.fill: parent
        visible: opacity > 0
    }

    Icon {
        anchors.centerIn: parent
        source: "image://theme/icon-l-video"
        color: "white"
        width: Theme.iconSizeLarge
        height: width
        visible: !fullscreen && !thumbnail.hasVisibleThumbnail  && controls.visible
        highlighted: videoMessageComponent.highlighted
    }


    BusyIndicator {
        id: videoBusyIndicator
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        visible: false
        running: visible
        size: BusyIndicatorSize.Medium
    }

    Timer {
        id: sliderTimer
        interval: 2000
    }
    Slider {
        id: messageVideoSlider
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Theme.paddingLarge
        minimumValue: 0
        maximumValue: messageVideo.duration ? messageVideo.duration : 0.1
        opacity: messageVideo.playbackState !== MediaPlayer.PlayingState || sliderTimer.running ? 1.0 : 0.0
        Behavior on opacity { FadeAnimation {} }
        highlighted: videoMessageComponent.highlighted || down
        stepSize: 1
        value: messageVideo.position
        enabled: messageVideo.seekable
        visible: (messageVideo.duration > 0) && opacity > 0
        onReleased: {
            messageVideo.seek(Math.floor(value));
            messageVideo.play();
        }
        valueText: getTimeString(Math.round((messageVideo.duration - messageVideo.position) / 1000))
    }

    Item {
        id: controls
        width: parent.width
        height: downloadingProgressBar.height + Theme.iconSizeLarge
        anchors.centerIn: parent
        visible: opacity > 0
        opacity: messageVideo.playbackState !== MediaPlayer.PlayingState ? 1.0 : 0.0
        IconButton {
            id: playButton
            anchors {
                left: parent.left
                right: fullscreenButton.left
            }

            height: Theme.iconSizeLarge
            icon {
                source: "image://theme/icon-l-play?white"
                asynchronous: true
            }
            highlighted: videoMessageComponent.highlighted || down
            visible: placeholderImage.status === Image.Ready ? true : false
            onClicked: {
                handlePlay();
            }
        }

        IconButton {
            id: fullscreenButton
            anchors {
                right: parent.right
                left: visible ? parent.horizontalCenter : parent.right
            }

            height: Theme.iconSizeLarge
            icon {
                asynchronous: true
                source: "../../../images/icon-l-fullscreen.svg"
                sourceSize {
                    width: Theme.iconSizeLarge
                    height: Theme.iconSizeLarge
                }
            }
            highlighted: videoMessageComponent.highlighted || down
            visible: (!videoMessageComponent.fullscreen) ? true : false
            onClicked: {
                pageStack.push(Qt.resolvedUrl("../../pages/VideoPage.qml"), {"contentBase": contentBase, isVideoNote: videoMessageComponent.isVideoNote});
            }
        }

        ProgressBar {
            id: downloadingProgressBar
            anchors.bottom: parent.bottom
            minimumValue: 0
            maximumValue: file.size
            value: file.downloadedSize
            visible: file.isDownloadingActive
            width: parent.width
        }
    }

    Rectangle {
        id: errorTextOverlay
        color: Theme.overlayBackgroundColor
        opacity: 0.8
        width: parent.width
        height: parent.height
        visible: false
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
    }

}
