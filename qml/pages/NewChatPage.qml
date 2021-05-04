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
import "../components"
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions
import "../js/debug.js" as Debug

Page {
    id: newChatPage
    allowedOrientations: Orientation.All

    property bool isLoading: true;
    property bool syncSupported: false;

    function resetFocus() {
        contactsSearchField.focus = false;
        newChatPage.focus = true;
    }

    function reloadContacts() {
        contactsModel.hydrateContacts();
        contactsListView.model = contactsProxyModel;
        newChatPage.isLoading = false;
    }

    onStatusChanged: {
        if (status === PageStatus.Active) {
            reloadContacts();
        }
    }

    Connections {
        target: tdLibWrapper
        onContactsImported: {
            reloadContacts();
            appNotification.show(qsTr("Contacts successfully synchronized with Telegram."));
        }
    }

    Component.onCompleted: {
        // With Sailfish OS 4 we can't get up-to-date contacts andmore. We might need to enter Sailjail eventually...
        // Details see https://forum.sailfishos.org/t/4-0-1-45-non-jailed-contacts-sqlite-database-no-longer-updated/4724
        var sailfishOSVersion = fernschreiberUtils.getSailfishOSVersion().split(".");
        if (parseInt(sailfishOSVersion[0]) < 4) {
            Debug.log("Sailfish OS version 3.x - contact sync should still be possible...")
            newChatPage.syncSupported = true;
        } else {
            Debug.log("Sailfish OS version 4.x - contact sync no longer supported...")
            newChatPage.syncSupported = false;
        }
    }

    SilicaFlickable {
        id: newChatContainer
        contentHeight: newChatPage.height
        anchors.fill: parent

        PullDownMenu {
            visible: contactsModel.canSynchronizeContacts() && newChatPage.syncSupported
            MenuItem {
                onClicked: {
                    newChatPage.isLoading = true;
                    if (!contactsModel.synchronizeContacts()) {
                        reloadContacts();
                        appNotification.show(qsTr("Could not synchronize your contacts with Telegram."));
                    }
                    // Success message is not fired before TDLib returned "Contacts imported" (see above)
                }
                text: qsTr("Synchronize Contacts with Telegram")
            }
        }

        Column {
            id: newChatPageColumn
            width: newChatPage.width
            height: newChatPage.height

            PageHeader {
                id: newChatPageHeader
                title: qsTr("Your Contacts")
            }

            Item {
                id: contactsItem

                width: newChatPageColumn.width
                height: newChatPageColumn.height - newChatPageHeader.height

                Column {
                    visible: !newChatPage.isLoading
                    width: parent.width
                    height: parent.height
                    SearchField {
                        id: contactsSearchField
                        width: parent.width
                        placeholderText: qsTr("Search a contact...")
                        active: !newChatPage.isLoading

                        onTextChanged: {
                            contactsProxyModel.setFilterWildcard("*" + text + "*");
                        }

                        EnterKey.iconSource: "image://theme/icon-m-enter-close"
                        EnterKey.onClicked: {
                            resetFocus();
                        }

                    }

                    SilicaListView {
                        id: contactsListView
                        clip: true
                        width: parent.width
                        height: parent.height - contactsSearchField.height
                        visible: !newChatPage.isLoading
                        opacity: visible ? 1 : 0
                        Behavior on opacity { FadeAnimation {} }

                        signal newChatInitiated ( int currentIndex )

                        ViewPlaceholder {
                            y: Theme.paddingLarge
                            enabled: contactsListView.count === 0
                            text: qsTr("You don't have any contacts.")
                        }

                        delegate: Item {
                            id: newChatListItem
                            width: parent.width
                            height: contactListItem.height

                            PhotoTextsListItem {
                                id: contactListItem

                                opacity: visible ? 1 : 0
                                Behavior on opacity { FadeAnimation {} }

                                pictureThumbnail {
                                    photoData: typeof photo_small !== "undefined" ? photo_small : {}
                                }
                                width: parent.width

                                primaryText.text: Emoji.emojify(title, primaryText.font.pixelSize, "../js/emoji/")
                                prologSecondaryText.text: "@" + ( username !== "" ? username : user_id )
                                tertiaryText {
                                    maximumLineCount: 1
                                    text: Functions.getChatPartnerStatusText(user_status, user_last_online);
                                }

                                onClicked: {
                                    contactsListView.newChatInitiated(index);
                                }

                                Connections {
                                    target: contactsListView

                                    onNewChatInitiated: {
                                        if (index === currentIndex) {
                                            contactListItem.visible = false;
                                        } else {
                                            contactListItem.visible = true;
                                        }
                                    }
                                }

                                Connections {
                                    target: contactsSearchField
                                    onFocusChanged: {
                                        if (contactsSearchField.focus) {
                                            contactListItem.visible = true;
                                        }
                                    }
                                }
                            }

                            Column {
                                id: selectChatTypeColumn
                                visible: !contactListItem.visible
                                opacity: visible ? 1 : 0
                                Behavior on opacity { FadeAnimation {} }
                                width: parent.width
                                height: contactListItem.height

                                Item {
                                    width: parent.width
                                    height: parent.height - chatTypeSeparator.height

                                    Rectangle {
                                        anchors.fill: parent
                                        opacity: Theme.opacityLow
                                        color: Theme.overlayBackgroundColor
                                    }

                                    Item {
                                        id: privateChatItem
                                        height: parent.height
                                        width: parent.width / 2

                                        Rectangle {
                                            id: privateChatHighlightBackground
                                            anchors.fill: parent
                                            color: Theme.highlightBackgroundColor
                                            opacity: Theme.opacityHigh
                                            visible: false
                                        }

                                        Row {
                                            width: parent.width
                                            height: parent.height - ( 2 * Theme.paddingSmall )
                                            anchors.verticalCenter: parent.verticalCenter

                                            IconButton {
                                                id: privateChatButton
                                                width: Theme.itemSizeLarge
                                                height: Theme.itemSizeLarge
                                                icon.source: "image://theme/icon-m-chat"
                                                anchors.verticalCenter: parent.verticalCenter
                                                onClicked: {
                                                    tdLibWrapper.createPrivateChat(display.id, "openDirectly");
                                                }
                                            }

                                            Column {
                                                height: parent.height
                                                width: parent.width - privateChatButton.width - Theme.horizontalPageMargin
                                                anchors.verticalCenter: parent.verticalCenter
                                                spacing: Theme.paddingSmall
                                                Text {
                                                    id: privateChatHeader
                                                    width: parent.width
                                                    font.pixelSize: Theme.fontSizeMedium
                                                    font.weight: Font.ExtraBold
                                                    color: Theme.primaryColor
                                                    maximumLineCount: 1
                                                    elide: Text.ElideRight
                                                    textFormat: Text.StyledText
                                                    text: qsTr("Private Chat")
                                                }
                                                Text {
                                                    width: parent.width
                                                    height: parent.height - privateChatHeader.height - Theme.paddingSmall
                                                    font.pixelSize: Theme.fontSizeTiny
                                                    color: Theme.secondaryColor
                                                    wrapMode: Text.Wrap
                                                    elide: Text.ElideRight
                                                    textFormat: Text.StyledText
                                                    text: qsTr("Transport-encrypted, uses Telegram Cloud, sharable across devices")
                                                }
                                            }

                                        }

                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                tdLibWrapper.createPrivateChat(display.id, "openDirectly");
                                            }
                                            onPressed: {
                                                privateChatHighlightBackground.visible = true;
                                            }
                                            onReleased: {
                                                privateChatHighlightBackground.visible = false;
                                            }
                                        }
                                    }

                                    Item {
                                        id: secretChatItem
                                        height: parent.height
                                        width: parent.width / 2
                                        anchors.left: privateChatItem.right
                                        anchors.top: parent.top

                                        Rectangle {
                                            id: secretChatHighlightBackground
                                            anchors.fill: parent
                                            color: Theme.highlightBackgroundColor
                                            opacity: Theme.opacityHigh
                                            visible: false
                                        }

                                        Row {
                                            width: parent.width
                                            height: parent.height - ( 2 * Theme.paddingSmall )
                                            anchors.verticalCenter: parent.verticalCenter

                                            IconButton {
                                                id: secretChatButton
                                                width: Theme.itemSizeLarge
                                                height: Theme.itemSizeLarge
                                                icon.source: "image://theme/icon-m-device-lock"
                                                anchors.verticalCenter: parent.verticalCenter
                                                onClicked: {
                                                    tdLibWrapper.createNewSecretChat(display.id, "openDirectly");
                                                }
                                            }

                                            Column {
                                                height: parent.height
                                                width: parent.width - secretChatButton.width - Theme.horizontalPageMargin
                                                anchors.verticalCenter: parent.verticalCenter
                                                spacing: Theme.paddingSmall
                                                Text {
                                                    width: parent.width
                                                    font.pixelSize: Theme.fontSizeMedium
                                                    font.weight: Font.ExtraBold
                                                    color: Theme.primaryColor
                                                    maximumLineCount: 1
                                                    elide: Text.ElideRight
                                                    textFormat: Text.StyledText
                                                    text: qsTr("Secret Chat")
                                                }
                                                Text {
                                                    width: parent.width
                                                    height: parent.height - privateChatHeader.height - Theme.paddingSmall
                                                    font.pixelSize: Theme.fontSizeTiny
                                                    color: Theme.secondaryColor
                                                    wrapMode: Text.Wrap
                                                    elide: Text.ElideRight
                                                    textFormat: Text.StyledText
                                                    text: qsTr("End-to-end-encrypted, accessible on this device only")
                                                }
                                            }

                                        }

                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                tdLibWrapper.createNewSecretChat(display.id, "openDirectly");
                                            }
                                            onPressed: {
                                                secretChatHighlightBackground.visible = true;
                                            }
                                            onReleased: {
                                                secretChatHighlightBackground.visible = false;
                                            }
                                        }
                                    }

                                }

                                Separator {
                                    id: chatTypeSeparator
                                    width: parent.width
                                    color: Theme.primaryColor
                                    horizontalAlignment: Qt.AlignHCenter
                                }

                            }

                        }

                        VerticalScrollDecorator {}
                    }

                }

                Column {

                    opacity: visible ? 1 : 0
                    Behavior on opacity { FadeAnimation {} }
                    visible: newChatPage.isLoading
                    width: parent.width
                    height: loadingLabel.height + loadingBusyIndicator.height + Theme.paddingMedium

                    spacing: Theme.paddingMedium

                    anchors.verticalCenter: parent.verticalCenter

                    InfoLabel {
                        id: loadingLabel
                        text: qsTr("Loading contacts...")
                    }

                    BusyIndicator {
                        id: loadingBusyIndicator
                        anchors.horizontalCenter: parent.horizontalCenter
                        running: newChatPage.isLoading
                        size: BusyIndicatorSize.Large
                    }
                }

            }

        }
    }
}
