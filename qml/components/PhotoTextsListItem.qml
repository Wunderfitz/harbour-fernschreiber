import QtQuick 2.6
import Sailfish.Silica 1.0
import WerkWolf.Fernschreiber 1.0

ListItem {
    id: chatListViewItem

    property alias primaryText: primaryText //usually chat name
    property alias prologSecondaryText: prologSecondaryText //usually last sender name
    property alias secondaryText: secondaryText //usually last message
    property alias tertiaryText: tertiaryText //usually last message date

    property int unreadCount: 0
    property bool isSecret: false
    property bool isVerified: false
    property bool isMarkedAsUnread: false
    property alias pictureThumbnail: pictureThumbnail

    contentHeight: mainRow.height + separator.height + 2 * Theme.paddingMedium
    contentWidth: parent.width

    Column {
        id: mainColumn
        width: parent.width - ( 2 * Theme.horizontalPageMargin )
        spacing: Theme.paddingSmall
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }

        Row {
            id: mainRow
            height: contentColumn.height
            spacing: Theme.paddingMedium

            Column {
                id: pictureColumn
                width: contentColumn.height - Theme.paddingSmall
                height: contentColumn.height - Theme.paddingSmall
                anchors.verticalCenter: parent.verticalCenter

                Item {
                    width: parent.width
                    height: parent.width

                    ProfileThumbnail {
                        id: pictureThumbnail
                        replacementStringHint: primaryText.text
                        width: parent.width
                        height: parent.width
                    }

                    Rectangle {
                        id: chatSecretBackground
                        color: Theme.overlayBackgroundColor
                        width: Theme.fontSizeExtraLarge
                        height: Theme.fontSizeExtraLarge
                        anchors.bottom: parent.bottom
                        radius: parent.width / 2
                        visible: chatListViewItem.isSecret
                    }

                    Image {
                        source: "image://theme/icon-s-secure"
                        height: Theme.fontSizeMedium
                        width: Theme.fontSizeMedium
                        anchors.centerIn: chatSecretBackground
                        visible: chatListViewItem.isSecret
                    }

                    Rectangle {
                        id: chatUnreadMessagesCountBackground
                        color: Theme.highlightBackgroundColor
                        width: Theme.fontSizeLarge
                        height: Theme.fontSizeLarge
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        radius: parent.width / 2
                        visible: chatListViewItem.unreadCount > 0 || chatListViewItem.isMarkedAsUnread
                    }

                    Text {
                        id: chatUnreadMessagesCount
                        font.pixelSize: Theme.fontSizeExtraSmall
                        font.bold: true
                        color: Theme.primaryColor
                        anchors.centerIn: chatUnreadMessagesCountBackground
                        visible: chatListViewItem.unreadCount > 0
                        text: chatListViewItem.unreadCount > 99 ? "99+" : chatListViewItem.unreadCount
                    }
                }
            }

            Column {
                id: contentColumn
                width: mainColumn.width - pictureColumn.width - mainRow.spacing
                spacing: Theme.paddingSmall

                Row {
                    id: primaryTextRow
                    spacing: Theme.paddingMedium

                    Label {
                        id: primaryText
                        textFormat: Text.StyledText
                        font.pixelSize: Theme.fontSizeMedium
                        truncationMode: TruncationMode.Fade
                        anchors.verticalCenter: parent.verticalCenter
                        width: Math.min(contentColumn.width - (verifiedImage.visible ? (verifiedImage.width + primaryTextRow.spacing) :  0), implicitWidth)
                    }

                    Image {
                        id: verifiedImage
                        anchors.verticalCenter: parent.verticalCenter
                        source: chatListViewItem.isVerified ? "../../images/icon-verified.svg" : ""
                        sourceSize.width: Theme.iconSizeExtraSmall
                        width: Theme.iconSizeExtraSmall
                        visible: status === Image.Ready
                    }
                }

                Row {
                    width: parent.width
                    spacing: Theme.paddingSmall
                    Label {
                        id: prologSecondaryText
                        font.pixelSize: Theme.fontSizeExtraSmall
                        width: Math.min(implicitWidth, parent.width)
                        color: Theme.highlightColor
                        textFormat: Text.StyledText
                        truncationMode: TruncationMode.Fade
                    }
                    Label {
                        id: secondaryText
                        font.pixelSize: Theme.fontSizeExtraSmall
                        width: parent.width - Theme.paddingMedium - prologSecondaryText.width
                        truncationMode: TruncationMode.Fade
                        textFormat: Text.StyledText
                        visible: prologSecondaryText.width < ( parent.width - Theme.paddingLarge )
                    }
                }

                Label {
                    id: tertiaryText
                    width: parent.width
                    font.pixelSize: Theme.fontSizeTiny
                    color: Theme.secondaryColor
                    truncationMode: TruncationMode.Fade
                }
            }
        }
    }

    Separator {
        id: separator
        anchors {
            top: mainColumn.bottom
            topMargin: Theme.paddingMedium
        }

        width: parent.width
        color: Theme.primaryColor
        horizontalAlignment: Qt.AlignHCenter
    }

}
