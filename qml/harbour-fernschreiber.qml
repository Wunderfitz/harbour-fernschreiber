import QtQuick 2.0
import Sailfish.Silica 1.0
import "pages"

ApplicationWindow
{
    id: appWindow

    Component {
        id: overviewPage
        OverviewPage {}
    }

    initialPage: overviewPage
    cover: Qt.resolvedUrl("pages/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations
}
