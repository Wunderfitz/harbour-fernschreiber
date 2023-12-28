import QtQuick 2.6
import Sailfish.Silica 1.0

SilicaFlickable {
    // id
    id: flickable
    // property declarations
    property real zoom
    property bool zoomed
    // override if needed
    property bool zoomEnabled: true
    property real minimumZoom: fitZoom
    property real maximumZoom: 4 //Math.max(fitZoom, 1) * 3

    default property alias zoomContentItem: zoomContentItem.data
    property alias implicitContentWidth: zoomContentItem.implicitWidth
    property alias implicitContentHeight: zoomContentItem.implicitHeight
    // factor for "PreserveAspectFit"
    readonly property real fitZoom: implicitContentWidth > 0 && implicitContentHeight > 0
        ? Math.min(maximumZoom, width / implicitContentWidth, height / implicitContentHeight)
        : 1.0
    readonly property int minimumBoundaryAxis: (implicitContentWidth / implicitContentHeight) > (width / height) ? Qt.Horizontal : Qt.Vertical

    // JavaScript functions
    function zoomOut(animated) {
        if (zoomed) {
            if(animated) { zoomOutAnimation.start() }
            else {
                zoom = fitZoom
                zoomed = false
            }
        }
    }

    // object properties
    contentWidth: Math.max(width, zoomContentItem.width)
    contentHeight: Math.max(height, zoomContentItem.height)
    enabled: !zoomOutAnimation.running && implicitContentWidth > 0 && implicitContentHeight > 0
    flickableDirection: Flickable.HorizontalAndVerticalFlick
    interactive: zoomed
    // According to Jolla, otherwise pinching would sometimes not work:
    pressDelay: 0
    Binding { // Update zoom on orientation changes and set as default
        target: flickable
        when: !zoomed
        property: "zoom"
        value: minimumZoom
    }
    // child objects

    PinchArea {
        id: pinchArea
        parent: flickable.contentItem
        width: flickable.contentWidth
        height: flickable.contentHeight
        enabled: zoomEnabled && minimumZoom !== maximumZoom && flickable.enabled
        onPinchUpdated: {
            scrollDecoratorTimer.restart()
            var f = flickable;
            var requestedZoomFactor = 1.0 + pinch.scale - pinch.previousScale;
            var previousWidth = f.contentWidth
            var previousHeight = f.contentHeight
            var targetWidth
            var targetHeight
            var targetZoom = requestedZoomFactor * f.zoom;
            if (targetZoom < f.minimumZoom) {
                f.zoom = f.minimumZoom;
                f.zoomed = false;
                f.contentX = 0;
                f.contentY = 0;
                return
            }  else if(targetZoom >= f.maximumZoom) {
                f.zoom = f.maximumZoom;
                targetHeight = f.implicitContentHeight * f.zoom
                targetWidth = f.implicitContentWidth * f.zoom
            }
            else if(targetZoom < f.maximumZoom) {
                if (f.minimumBoundaryAxis == Qt.Horizontal) {
                    targetWidth = f.contentWidth * requestedZoomFactor
                    f.zoom = targetWidth / f.implicitContentWidth
                    targetHeight = f.implicitContentHeight * f.zoom
                } else {
                    targetHeight = f.contentHeight * requestedZoomFactor
                    f.zoom = targetHeight / f.implicitContentHeight
                    targetWidth = f.implicitContentWidth * f.zoom
                }
            }
            // calculate center difference
            f.contentX += pinch.previousCenter.x - pinch.center.x
            f.contentY += pinch.previousCenter.y - pinch.center.y
            // move to new (zoomed) center. this jumps a tiny bit, but is bearable:
            if (targetWidth > f.width)
                f.contentX -= (previousWidth - targetWidth)/(previousWidth/pinch.previousCenter.x)
            if (targetHeight > f.height)
                f.contentY -= (previousHeight - targetHeight)/(previousHeight/pinch.previousCenter.y)

            f.zoomed = true
        }
        onPinchFinished: {
            returnToBounds()
        }
        Item {
            id: zoomContentItem
            anchors.centerIn: parent
            implicitWidth: flickable.width
            implicitHeight: flickable.height
            width: Math.ceil(implicitWidth * zoom)
            height: Math.ceil(implicitHeight * zoom)
        }
    }
    // enable zoom to minimumZoom on click
    ParallelAnimation {
        id: zoomOutAnimation
        NumberAnimation {
            target: flickable
            properties: "contentX, contentY"
            to: 0
        }
        NumberAnimation {
            target: flickable
            property: "zoom"
            to: fitZoom
        }
        onRunningChanged: {
            if(!running) {
                zoomed = false
            }
        }
    }

    // show scroll decorators when scrolling OR zooming
    Timer {
        id: scrollDecoratorTimer
        readonly property bool moving: flickable.moving
        readonly property bool showing: moving || running
        onMovingChanged: restart()
        interval: 300
    }

    VerticalScrollDecorator {
        flickable: flickable
        opacity: scrollDecoratorTimer.showing ? 1.0 : 0.0
    }
    HorizontalScrollDecorator {
        flickable: flickable
        opacity: scrollDecoratorTimer.showing ? 1.0 : 0.0
    }
}
