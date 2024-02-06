import QtQuick 2.6
import Sailfish.Silica 1.0
import WerkWolf.Fernschreiber 1.0
import QtMultimedia 5.6
import QtGraphicalEffects 1.0
import "../../"

Video {
    id: video
    property var videoData: model.modelData.content.video
    readonly property bool isPlaying: playbackState === MediaPlayer.PlayingState
    readonly property bool isCurrent: index === page.index
    property bool shouldPlay
    autoLoad: true
    source: file.isDownloadingCompleted ? file.path : ''
    onIsCurrentChanged: {
        if(!isCurrent) {
            pause()
        }
    }
    onStatusChanged: {
        if(status === MediaPlayer.EndOfMedia) {
            page.overlayActive = true
        }
    }
    TDLibThumbnail {
        id: tdLibImage

        property bool active: !file.isDownloadingCompleted || (!video.isPlaying && (video.position === 0 || video.status === MediaPlayer.EndOfMedia))
        opacity: active ? 1 : 0
        visible: active || opacity > 0

        width: parent.width //don't use anchors here for easier custom scaling
        height: parent.height
//        highlighted: parent.highlighted
        thumbnail: videoData.thumbnail
        minithumbnail: videoData.minithumbnail
        fillMode: Image.PreserveAspectFit


    }

    TDLibFile {
        id: file
        autoLoad: false
        tdlib: tdLibWrapper
        fileInformation: videoData.video
        property real progress: isDownloadingCompleted ? 1.0 : (downloadedSize / size)
        onDownloadingCompletedChanged: {
            if(isDownloadingCompleted) {
                video.source = file.path
                if(video.shouldPlay) {
                    video.play()
                    delayedOverlayHide.start()
                    video.shouldPlay = false
                }
            }
        }
    }
    Label {
        anchors.centerIn: parent
        text: 'dl: '+file.downloadedSize
              + ' \ns: '+file.size
              + ' \nes: '+file.expectedSize
              + ' \nd:'+file.isDownloadingActive
              + ' \nc:'+file.isDownloadingCompleted

    }

    MouseArea {
        anchors.fill: parent
        onClicked: page.overlayActive = !page.overlayActive
    }

    RadialGradient { // white videos = invisible button. I can't tell since which SFOS version the opaque button is available, so:
        id: buttonBg
        anchors.centerIn: parent
        width: Theme.itemSizeLarge; height: Theme.itemSizeLarge
        property color baseColor: Theme.rgba(palette.overlayBackgroundColor, 0.2)

        enabled: videoUI.active || !file.isDownloadingCompleted
        opacity: enabled ? 1 : 0
        Behavior on opacity { FadeAnimator {} }
        gradient: Gradient {

            GradientStop { position: 0.0; color: buttonBg.baseColor }
            GradientStop { position: 0.3; color: buttonBg.baseColor }
            GradientStop { position: 0.5; color: 'transparent' }
        }

        IconButton {
            anchors.fill: parent
            icon.source: "image://theme/icon-l-"+(video.isPlaying || video.shouldPlay ? 'pause' : 'play')+"?" + (pressed
                         ? Theme.highlightColor
                         : Theme.lightPrimaryColor)
            onClicked: {
                if (!file.isDownloadingCompleted) {
                    video.shouldPlay = !video.shouldPlay;
                    if(video.shouldPlay) {
                        file.load()
                    } else {
                        file.cancel()
                    }
                    return;
                }

                if (video.isPlaying) {
                    video.pause()
                } else {
                    video.play()
                    delayedOverlayHide.start()
                }
            }
        }
    }

    ProgressCircle {
        property bool active: file.isDownloadingActive
        opacity: active ? 1 : 0
        Behavior on opacity { FadeAnimator {} }
        anchors.centerIn: parent
        value: file.progress
    }
    Item {
        id: videoUI
        property bool active: overlay.active// && file.isDownloadingCompleted
        anchors.fill: parent
        opacity: active ? 1 : 0
        Behavior on opacity { FadeAnimator {} }

        Slider {
            id: slider
            value: video.position
            minimumValue: 0
            maximumValue: video.duration || 0.1
            enabled: parent.active && video.seekable
            width: parent.width
            handleVisible: false
            animateValue: true
            stepSize: 500
            anchors {
                bottom: parent.bottom
                bottomMargin: Theme.itemSizeMedium
            }
            valueText: value > 0 || down ? Format.formatDuration((value)/1000, Formatter.Duration) : ''
            leftMargin: Theme.horizontalPageMargin
            rightMargin: Theme.horizontalPageMargin
            onDownChanged: {
                if(!down) {
                    video.seek(value)
                    value = Qt.binding(function() { return video.position })
                }
            }
            Label {
                anchors {
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                    bottom: parent.bottom
                    topMargin: Theme.paddingSmall
                }
                font.pixelSize: Theme.fontSizeExtraSmall
                text: file.isDownloadingCompleted
                      ? Format.formatDuration((parent.maximumValue - parent.value)/1000, Formatter.Duration)
                      : (video.videoData.duration
                        ? Format.formatDuration(video.videoData.duration, Formatter.Duration) + ', '
                        : '') + Format.formatFileSize(file.size || file.expectedSize)
                color: Theme.secondaryColor
            }
        }

        Timer {
            id: delayedOverlayHide
            interval: 500
            onTriggered: {
                if(video.isPlaying) {
                    page.overlayActive = false
                }
            }
        }
    }
}
