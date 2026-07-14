import QtQuick 2.0
import Sailfish.Silica 1.0
import WerkWolf.Fernschreiber 1.0
import "../../"

ZoomArea {
    // id
    id: zoomArea
    property var photoData //albumMessages[index].content.photo
    property bool active: true
    property alias image: image
    property bool highlighted

    signal clicked

    maximumZoom: Math.max(Screen.width, Screen.height) / 200
//    maximumZoom: Math.max(fitZoom, 1) * 3
    implicitContentWidth: image.implicitWidth
    implicitContentHeight: image.implicitHeight
    zoomEnabled: image.status == Image.Ready

    onActiveChanged: {
        if (!active) {
            zoomOut()
        }
    }

    Component.onCompleted: {
//        var photoData = albumMessages[index].content.photo;
        if (photoData) {

            var biggestIndex = -1
            for (var i = 0; i < photoData.sizes.length; i++) {
                if (biggestIndex === -1 || photoData.sizes[i].width > photoData.sizes[biggestIndex].width) {
                    biggestIndex = i;
                }
            }
            if (biggestIndex > -1) {
                image.sourceSize.width = photoData.sizes[biggestIndex].width
                image.sourceSize.height = photoData.sizes[biggestIndex].height
                image.fileInformation = photoData.sizes[biggestIndex].photo
            }
        }
    }
    TDLibImage {
        id: image

        width: parent.width
        height: parent.height
        source: file.isDownloadingCompleted ? file.path : ""
        anchors.centerIn: parent

        fillMode: Image.PreserveAspectFit
        asynchronous: true
        smooth: !(movingVertically || movingHorizontally)


        Behavior on opacity { FadeAnimator{} }
    }
    Item {
        anchors.fill: parent

    }
    MouseArea {
        id: mouseArea
        anchors.centerIn: parent
        width: zoomArea.contentWidth
        height: zoomArea.contentHeight
        onClicked: zoomArea.clicked()
    }


    BusyIndicator {
        running: image.file.isDownloadingActive && !delayBusyIndicator.running
        size: BusyIndicatorSize.Large
        anchors.centerIn: parent
        parent: zoomArea
        Timer {
            id: delayBusyIndicator
            running: image.file.isDownloadingActive
            interval: 1000
        }
    }
}
