
import QtQuick 2.6

ZoomImage {
    photoData: model.modelData.content.photo
    onClicked: {
        console.log('clicked', zoomed)
        if(zoomed) {
            zoomOut(true)
            page.overlayActive = true
        } else {
            page.overlayActive = !page.overlayActive
        }
    }

}
