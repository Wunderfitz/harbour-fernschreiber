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
import WerkWolf.Fernschreiber 1.0
import "../"
import "../../js/twemoji.js" as Emoji

InlineQueryResult {
    id: queryResultItem
    property var resultData: model.audio || model.voice_note
    property var audioData: resultData.audio || resultData.voice

    enabled: false // don't send on click

    Connections {
        target: resultView
        onRequestPlayback: {
            if(audioPlayer.playbackState === Audio.PlayingState && audioPlayer.source !== playbackSource) {
                audioPlayer.pause()
            }
        }
    }

    TDLibFile {
        id: file
        tdlib: tdLibWrapper
        autoLoad: false
        fileInformation: queryResultItem.audioData
    }

    TDLibFile {
        id: thumbnail
        tdlib: tdLibWrapper
        autoLoad: true
        fileInformation: queryResultItem.resultData.album_cover_thumbnail ? queryResultItem.resultData.album_cover_thumbnail.file : {}
    }

    Loader {
        id: thumbnailLoader
        asynchronous: true
        active: thumbnail.isDownloadingCompleted
        height: parent.height
        width: height
        opacity: item && item.status === Image.Ready ? 0.5 : 0.0
        Behavior on opacity { FadeAnimation {} }
        sourceComponent: Component {
            Image {
                id: thumbnailImage
                source: thumbnail.path
                sourceSize.width: width
                sourceSize.height: height

                layer.enabled: playPauseButton.pressed
                layer.effect: PressEffect { source: thumbnailImage }
            }
        }
    }

    IconButton {
        id: playPauseButton
        anchors.centerIn: thumbnailLoader
        icon {
            asynchronous: true
            source: audioPlayer.playbackState === Audio.PlayingState || (file.isDownloadingActive && audioPlayer.autoPlay) ? "image://theme/icon-m-pause": "image://theme/icon-m-play"
        }
        onClicked: {
            if(!file.isDownloadingCompleted && !file.isDownloadingActive) {
                file.load();
                audioPlayer.autoPlay = true
            } else if(file.isDownloadingActive) {
                // cancel playback intent?
                audioPlayer.autoPlay = false
            } else if(file.isDownloadingCompleted) {
                //playPause
                if(audioPlayer.playbackState === Audio.PlayingState) {
                    audioPlayer.pause();
                } else {
                    audioPlayer.play();
                }
            }
        }
    }
    ProgressCircle {
        value: file.downloadedSize / file.expectedSize
        width: Theme.iconSizeMedium
        height: Theme.iconSizeMedium
        anchors.centerIn: playPauseButton
        opacity: file.isDownloadingActive ? 1.0 : 0.0
        Behavior on opacity { FadeAnimation {} }
    }

    Audio {
        id: audioPlayer
        source: file.isDownloadingCompleted ? file.path : ""
        autoPlay: false
        onPlaybackStateChanged: {
            if(playbackState === Audio.PlayingState) {
                resultView.requestPlayback(source);
            }
        }
    }

    Column {
        anchors {
            left: thumbnailLoader.right
            leftMargin: Theme.paddingSmall
            right: sendButton.left
            verticalCenter: parent.verticalCenter
        }

        Label {
            width: parent.width
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.highlightColor
            text: Emoji.emojify(queryResultItem.resultData.performer || "", font.pixelSize)
            visible: text.length > 0
            truncationMode: TruncationMode.Fade
        }
        Label {
            width: parent.width
            font.pixelSize: Theme.fontSizeTiny
            color: Theme.secondaryHighlightColor
            text: Emoji.emojify(queryResultItem.resultData.title || model.title || "", font.pixelSize)
            visible: text.length > 0
            truncationMode: TruncationMode.Fade
        }
        Item {
            height: sizeLabel.height
            width: parent.width
            Label {
                id: durationLabel
                font.pixelSize: Theme.fontSizeTiny
                color: Theme.secondaryColor
                text: (audioPlayer.position > 0 || audioPlayer.playbackState === Audio.PlayingState ? (Format.formatDuration(audioPlayer.position/1000, Formatter.DurationShort)+" / ") : "") + Format.formatDuration(queryResultItem.audioData.duration || (audioPlayer.duration/1000), Formatter.DurationShort)
                visible: (queryResultItem.audioData.duration || (audioPlayer.duration/1000)) > 0
                truncationMode: TruncationMode.Fade
            }
            Label {
                id: sizeLabel
                anchors.right: parent.right
                font.pixelSize: Theme.fontSizeTiny
                color: Theme.secondaryColor
                text: Format.formatFileSize(file.expectedSize)
                visible: file.expectedSize > 0
                truncationMode: TruncationMode.Fade
            }
        }
    }

    IconButton {
        id: sendButton
        anchors {
            right: parent.right
            rightMargin: Theme.horizontalPageMargin
            verticalCenter: parent.verticalCenter
        }
        icon {
            asynchronous: true
            source: "image://theme/icon-m-send"
        }
        onClicked: {
            queryResultItem.sendInlineQueryResultMessage();
        }
    }
}
