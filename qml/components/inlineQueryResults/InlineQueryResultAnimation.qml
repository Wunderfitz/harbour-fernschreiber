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
import QtGraphicalEffects 1.0
import Nemo.Thumbnailer 1.0
import "../"
import "../../js/twemoji.js" as Emoji
import "../../js/debug.js" as Debug

InlineQueryResult {
    id: queryResultItem
    property bool isAnimation: true
    property bool loopPreview: isAnimation
    property bool mutePreview: isAnimation
    enabled: false // don't send on click
    layer.enabled: mouseArea.pressed
    layer.effect: PressEffect { source: queryResultItem }

    property string animationKey: "animation"
    property bool hasThumbnail: !!model[queryResultItem.animationKey].thumbnail

    property string videoMimeType: "video/mp4"

    TDLibFile {
        id: file
        tdlib: tdLibWrapper
        autoLoad: true
        fileInformation: hasThumbnail ? model[queryResultItem.animationKey].thumbnail.file : (queryResultItem.isAnimation ? model[queryResultItem.animationKey].animation : model[queryResultItem.animationKey].video)
    }

    Image {
        id: miniThumbnail
        asynchronous: true
        source: model[queryResultItem.animationKey].minithumbnail ? "data:image/jpg;base64,"+model[queryResultItem.animationKey].minithumbnail.data : ""
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        layer.enabled: queryResultItem.pressed
        layer.effect: PressEffect { source: miniThumbnail }
    }
    Component {
        id: videoThumbnail
        Thumbnail {
            id: thumbnail
            source: file.path
            sourceSize.width: width
            sourceSize.height: height
            mimeType: queryResultItem.videoMimeType
            layer.enabled: queryResultItem.pressed
            layer.effect: PressEffect { source: thumbnail }
            opacity: status === Thumbnail.Ready ? 1.0 : 0.0
            Behavior on opacity { FadeAnimation {} }
        }
    }
    Component {
        id: imageThumbnail
        Image {
            id: thumbnail
            source: file.path
            sourceSize.width: width
            sourceSize.height: height
            layer.enabled: queryResultItem.pressed
            layer.effect: PressEffect { source: thumbnail }
            fillMode: Image.PreserveAspectCrop
            opacity: status === Image.Ready ? 1.0 : 0.0
            Behavior on opacity { FadeAnimation {} }
            onStatusChanged: {
                // we don't get many hints what may be wrong, so we guess it may be a webp image ;)
                if(status === Image.Error) {
                    Debug.log("Inline Query Video: Thumbnail invalid. Blindly trying webp, which might work.")
                    queryResultItem.videoMimeType = "image/webp";
                    thumbnailLoader.sourceComponent = videoThumbnail;
                }
            }
        }
    }
    Loader {
        id: thumbnailLoader
        asynchronous: true
        active: file.isDownloadingCompleted
        anchors.fill: parent
        sourceComponent: queryResultItem.hasThumbnail ? (model[queryResultItem.animationKey].thumbnail.format["@type"] === "thumbnailFormatMpeg4" ? videoThumbnail : imageThumbnail) : model[queryResultItem.animationKey].mime_type === "video/mp4" ? videoThumbnail : imageThumbnail
    }
    Column {
        id: texts
        anchors {
            left: parent.left
            margins: Theme.paddingSmall
            right: parent.right
            bottom: parent.bottom
        }

        Label {
            id: titleLabel
            width: parent.width
            font.pixelSize: Theme.fontSizeTiny
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            visible: text.length > 0
            text: Emoji.emojify(model.title || "", font.pixelSize);
        }
        Label {
            id: descriptionLabel
            width: parent.width
            font.pixelSize: Theme.fontSizeTiny
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            visible: text.length > 0
            text: Emoji.emojify(model.description || "", font.pixelSize);
        }
    }

    Loader {
        anchors.fill: texts
        asynchronous: true
        active: titleLabel.visible || descriptionLabel.visible
        sourceComponent: Component {
            DropShadow {
                horizontalOffset: 0
                verticalOffset: 0
                radius: Theme.paddingSmall
                spread: 0.5
                samples: 17
                color: Theme.overlayBackgroundColor
                source: texts
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            // dialog

            var dialog = pageStack.push(dialogComponent,{})
            dialog.accepted.connect(function() {
                queryResultItem.sendInlineQueryResultMessage();
            })
        }
    }
    Component {
        id: dialogComponent
        Dialog {

            TDLibFile {
                id: previewFile
                tdlib: tdLibWrapper
                autoLoad: model[queryResultItem.animationKey].mime_type !== "text/html"
                fileInformation: queryResultItem.isAnimation ? model[queryResultItem.animationKey].animation : model[queryResultItem.animationKey].video
            }

            DialogHeader { id: dialogHeader }

            ProgressCircle {
                value: previewFile.downloadedSize / previewFile.expectedSize
                width: Theme.iconSizeMedium
                height: Theme.iconSizeMedium
                anchors.centerIn: parent
                opacity: previewFile.isDownloadingActive ? 1.0 : 0.0
                Behavior on opacity { FadeAnimation {} }
            }
            Column {
                visible: !previewFile.autoLoad
                spacing: Theme.paddingLarge
                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                    verticalCenter: parent.verticalCenter
                }

                Label {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: Theme.secondaryHighlightColor
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: Emoji.emojify(model.title || "", font.pixelSize);
                    visible: text.length > 1
                    linkColor: Theme.primaryColor
                }

                Label {
                    width: parent.width
                    font.pixelSize: Theme.fontSizeLarge
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: Theme.highlightColor
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: '<a href="'+Emoji.emojify(previewFile.fileInformation.remote.id, font.pixelSize)+'">'+Emoji.emojify(previewFile.fileInformation.remote.id, font.pixelSize)+'</a> '
                    linkColor: Theme.primaryColor
                }

                Label {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: Theme.secondaryHighlightColor
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: Emoji.emojify(model.description || "", font.pixelSize)
                    visible: text.length > 1
                    linkColor: Theme.secondaryColor
                }
            }


            Loader {
                id: videoLoader
                anchors {
                    top: dialogHeader.bottom
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                active: previewFile.isDownloadingCompleted
                asynchronous: true
                sourceComponent: Component {
                    Item {
                        Connections {
                            target: resultView
                            onRequestPlayback: {
                                if(previewVideo.playbackState === MediaPlayer.PlayingState && previewVideo.source !== playbackSource) {
                                    previewVideo.pause()
                                }
                            }
                        }
                        Timer {
                            id: loopTimer
                            interval: 0
                            onTriggered: previewVideo.play()
                        }

                        Video {
                            id: previewVideo
                            source: previewFile.path
                            autoPlay: true
                            muted: queryResultItem.mutePreview
                            anchors.fill: parent

                            onStatusChanged: {
                                if (status == MediaPlayer.EndOfMedia) {
                                    if(queryResultItem.loopPreview) {
                                        loopTimer.start()
                                    }
                                }
                            }
                            onPlaybackStateChanged: {
                                if(playbackState === MediaPlayer.PlayingState) {
                                    resultView.requestPlayback(source);
                                }
                            }
                            layer.enabled: playPauseMouseArea.pressed
                            layer.effect: PressEffect { source: previewVideo }
                        }
                        MouseArea {
                            id: playPauseMouseArea
                            anchors.fill: parent
                            onClicked: {
                                if(previewVideo.playbackState === MediaPlayer.PlayingState) {
                                    previewVideo.pause();
                                } else {
                                    previewVideo.play();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
