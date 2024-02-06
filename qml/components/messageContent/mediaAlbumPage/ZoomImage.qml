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
//                imageDelegate.imageWidth = photoData.sizes[biggestIndex].width;
//                imageDelegate.imageHeight = photoData.sizes[biggestIndex].height;
                image.sourceSize.width = photoData.sizes[biggestIndex].width
                image.sourceSize.height = photoData.sizes[biggestIndex].height
                image.fileInformation = photoData.sizes[biggestIndex].photo

                console.log('loading photo', JSON.stringify(image.fileInformation))
            }
        }
    }
    TDLibImage {
        id: image

        width: parent.width
        height: parent.height
        source: file.isDownloadingCompleted ? file.path : ""
//        enabled: true //!!file.fileId
//        anchors.fill: parent
        anchors.centerIn: parent

        fillMode: Image.PreserveAspectFit
        asynchronous: true
        smooth: !(movingVertically || movingHorizontally)

//        sourceSize.width: Screen.height
//        visible: opacity > 0
//        opacity: status === Image.Ready ? 1 : 0

        Behavior on opacity { FadeAnimator{} }
    }
//    Label {
//        anchors.fill: parent
//        text: 'ok?' + image.enabled +' fileid:' +!!(image.file.fileId)
//              + '\n - dl?' + image.file.isDownloadingActive
//              + '\n completed?' + image.file.isDownloadingCompleted + ' path:'+ image.file.path
//              + '\n ' + image.source
//        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
//    }
//    Rectangle {
//        color: 'green'
//        anchors.fill: image
//        opacity: 0.3

//    }

//    Image {
//        id: image
//        anchors.fill: parent
//        smooth: !(movingVertically || movingHorizontally)
//        sourceSize.width: Screen.height
//        fillMode: Image.PreserveAspectFit
//        asynchronous: true
//        cache: false

//        onSourceChanged: {
//            zoomOut()
//        }

//        opacity: status == Image.Ready ? 1 : 0
//        Behavior on opacity { FadeAnimator{} }
//    }
    Item {
        anchors.fill: parent

    }
    MouseArea {
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
//    Rectangle {
//        color: 'green'
//        anchors.fill: parent
//        parent: zoomArea
//    }
}
