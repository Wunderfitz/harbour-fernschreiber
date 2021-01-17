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
import "../"
import "../../js/twemoji.js" as Emoji
import "../../js/functions.js" as Functions
import "../../js/debug.js" as Debug


MessageContentFileInfoBase {
    id: contentItem

    fileInformation: rawMessage.content.audio.audio
    thumbnail: rawMessage.content.audio.album_cover_thumbnail
    minithumbnail: rawMessage.content.audio.album_cover_minithumbnail

    primaryText: Emoji.emojify(rawMessage.content.audio.performer, primaryLabel.font.pixelSize)
    secondaryText: Emoji.emojify(rawMessage.content.audio.title, secondaryLabel.font.pixelSize)
    tertiaryLabel.visible: (duration || (audioPlayer.duration/1000)) > 0
    tertiaryText: (audioPlayer.position > 0 || audioPlayer.playbackState === Audio.PlayingState ? (Format.formatDuration(audioPlayer.position/1000, Formatter.DurationShort)+" / ") : "") + Format.formatDuration(contentItem.duration > 0 ? contentItem.duration : (audioPlayer.duration/1000), Formatter.DurationShort)

    leftButton {
        icon.source: audioPlayer.playbackState === Audio.PlayingState || (file.isDownloadingActive && audioPlayer.autoPlay) ? "image://theme/icon-m-pause": "image://theme/icon-m-play"
        onClicked: {
            if(!file.isDownloadingCompleted && !file.isDownloadingActive) {
                file.load();
                audioPlayer.autoPlay = true;
            } else if(file.isDownloadingActive) {
                audioPlayer.autoPlay = false;
                file.cancel();
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

    property int duration: rawMessage.content.audio.duration

    Audio {
        id: audioPlayer
        source: file.isDownloadingCompleted ? file.path : ""
        autoPlay: false
    }

    Slider {
        width: parent.width
        anchors {
            left: parent.left
            leftMargin: -Screen.width/16
            right: parent.right
            rightMargin: -Screen.width/16
            top: primaryItem.bottom
            topMargin: -height/3
        }
        minimumValue: 0
        maximumValue: audioPlayer.duration ? audioPlayer.duration : 0.1
        stepSize: 1
        value: audioPlayer.position
        enabled: audioPlayer.seekable
        visible: file.isDownloadingCompleted && audioPlayer.playbackState === Audio.PlayingState || audioPlayer.playbackState === Audio.PausedState
        opacity: visible ? 1.0 : 0.0
        Behavior on opacity { FadeAnimation {} }
        height: visible ? implicitHeight : 0
        Behavior on height { NumberAnimation { duration: 200 } }

        highlighted: contentItem.highlighted || down
        onReleased: {
            audioPlayer.seek(Math.floor(value));
            audioPlayer.play();
        }
    }
}
