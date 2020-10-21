import QtQuick 2.5
import Sailfish.Silica 1.0
ListItem {

    id: chatListViewItem

    property alias primaryText: primaryText //usually chat name
    property alias prologSecondaryText: prologSecondaryText //usually last sender name
    property alias secondaryText: secondaryText //usually last message
    property alias tertiaryText: tertiaryText //usually last message date

    property int unreadCount
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
            width: parent.width
            height: contentColumn.height
            spacing: Theme.paddingMedium

            Column {
                id: pictureColumn
                width: contentColumn.height - Theme.paddingSmall
                height: contentColumn.height - Theme.paddingSmall
                anchors.verticalCenter: parent.verticalCenter

                Item {
                    id: chatListPictureItem
                    width: parent.width
                    height: parent.width

                    ProfileThumbnail {
                        id: pictureThumbnail
                        replacementStringHint: primaryText.text
                        width: parent.width
                        height: parent.width
                    }

                    Rectangle {
                        id: chatUnreadMessagesCountBackground
                        color: Theme.highlightBackgroundColor
                        width: Theme.fontSizeLarge
                        height: Theme.fontSizeLarge
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        radius: parent.width / 2
                        visible: chatListViewItem.unreadCount > 0
                    }

                    Text {
                        id: chatUnreadMessagesCount
                        font.pixelSize: Theme.fontSizeExtraSmall
                        font.bold: true
                        color: Theme.primaryColor
                        anchors.centerIn: chatUnreadMessagesCountBackground
                        visible: chatUnreadMessagesCountBackground.visible
                        text: chatListViewItem.unreadCount > 99 ? "99+" : chatListViewItem.unreadCount
                    }
                }
            }

            Column {
                id: contentColumn
                width: parent.width * 5 / 6 - Theme.horizontalPageMargin
                spacing: Theme.paddingSmall

                Text {
                    id: primaryText
                    textFormat: Text.StyledText
                    font.pixelSize: Theme.fontSizeMedium
                    color: Theme.primaryColor
                    elide: Text.ElideRight
                    width: parent.width
                    onTruncatedChanged: {
                        // There is obviously a bug in QML in truncating text with images.
                        // We simply remove Emojis then...
                        if (truncated) {
                            text = text.replace(/\<img [^>]+\/\>/g, "");
                        }
                    }
                }

                Row {
                    id: additionalTextRow
                    width: parent.width
                    spacing: Theme.paddingSmall
                    Text {
                        id: prologSecondaryText
                        font.pixelSize: Theme.fontSizeExtraSmall
                        color: Theme.highlightColor
                        textFormat: Text.StyledText
                        onTruncatedChanged: {
                            // There is obviously a bug in QML in truncating text with images.
                            // We simply remove Emojis then...
                            if (truncated) {
                                text = text.replace(/\<img [^>]+\/\>/g, "");
                            }
                        }
                    }
                    Text {
                        id: secondaryText
                        font.pixelSize: Theme.fontSizeExtraSmall
                        color: Theme.primaryColor
                        width: parent.width - Theme.paddingMedium - prologSecondaryText.width
                        elide: Text.ElideRight
                        textFormat: Text.StyledText
                        onTruncatedChanged: {
                            // There is obviously a bug in QML in truncating text with images.
                            // We simply remove Emojis then...
                            if (truncated) {
                                text = text.replace(/\<img [^>]+\/\>/g, "");
                            }
                        }
                    }
                }

                Text {
                    id: tertiaryText
                    width: parent.width
                    font.pixelSize: Theme.fontSizeTiny
                    color: Theme.secondaryColor
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
