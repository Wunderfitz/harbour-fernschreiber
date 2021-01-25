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
import WerkWolf.Fernschreiber 1.0
import "../components"
import "../js/functions.js" as Functions
import "../js/debug.js" as Debug

Page {
    id: settingsPage
    allowedOrientations: Orientation.All

    readonly property bool landscapeLayout: (width > height && Screen.sizeCategory > Screen.Small) || Screen.sizeCategory > Screen.Medium
    readonly property var userInformation: tdLibWrapper.getUserInformation()

    onStatusChanged: {
        if (status === PageStatus.Active) {
            tdLibWrapper.getUserProfilePhotos(userInformation.id, 100, 0);
        }
    }

    Connections {
        target: tdLibWrapper
        onOwnUserUpdated: {
            firstNameEditArea.text = userInformation.first_name;
            lastNameEditArea.text = userInformation.last_name;
            userNameEditArea.text = userInformation.username;
        }
        onUserProfilePhotosReceived: {
            if (extra === userInformation.id.toString()) {
                imageContainer.thumbnailModel = photos;
            }
        }
    }

    SilicaFlickable {
        id: settingsContainer
        contentHeight: column.height
        anchors.fill: parent

        Column {
            id: column
            width: settingsPage.width

            PageHeader {
                title: qsTr("Settings")
            }

            SectionHeader {
                text: qsTr("User Profile")
            }

            Grid {
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                columns: landscapeLayout ? 2 : 1
                columnSpacing: Theme.horizontalPageMargin
                anchors.horizontalCenter: parent.horizontalCenter

                readonly property real columnWidth: width/columns

                InformationEditArea {
                    id: firstNameEditArea
                    visible: true
                    canEdit: true
                    headerText: qsTr("First Name", "first name of the logged-in profile - header")
                    text: userInformation.first_name
                    width: parent.columnWidth
                    headerLeftAligned: true

                    onSaveButtonClicked: {
                        if(!editItem.errorHighlight) {
                            tdLibWrapper.setName(textValue, lastNameEditArea.text);
                        } else {
                            isEditing = true;
                        }
                    }

                    onTextEdited: {
                        if(textValue.length > 0 && textValue.length < 65) {
                            editItem.errorHighlight = false;
                            editItem.label = "";
                            editItem.placeholderText = "";
                        } else {
                            editItem.label = qsTr("Enter 1-64 characters");
                            editItem.placeholderText = editItem.label;
                            editItem.errorHighlight = true;
                        }
                    }
                }

                InformationEditArea {
                    id: lastNameEditArea
                    visible: true
                    canEdit: true
                    headerText: qsTr("Last Name", "last name of the logged-in profile - header")
                    text: userInformation.last_name
                    width: parent.columnWidth
                    headerLeftAligned: true

                    onSaveButtonClicked: {
                        if(!editItem.errorHighlight) {
                            tdLibWrapper.setName(firstNameEditArea.text, textValue);
                        } else {
                            isEditing = true;
                        }
                    }

                    onTextEdited: {
                        if(textValue.length >= 0 && textValue.length < 65) {
                            editItem.errorHighlight = false;
                            editItem.label = "";
                            editItem.placeholderText = "";
                        } else {
                            editItem.label = qsTr("Enter 0-64 characters");
                            editItem.placeholderText = editItem.label;
                            editItem.errorHighlight = true;
                        }
                    }
                }

                InformationEditArea {
                    id: userNameEditArea
                    visible: true
                    canEdit: true
                    headerText: qsTr("Username", "user name of the logged-in profile - header")
                    text: userInformation.username
                    width: parent.columnWidth
                    headerLeftAligned: true

                    onSaveButtonClicked: {
                        tdLibWrapper.setUsername(textValue);
                    }
                }

            }

            SectionHeader {
                horizontalAlignment: Text.AlignLeft
                text: qsTr("Profile Pictures")
            }

            Row {
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                spacing: Theme.paddingMedium

                Item {
                    id: imageContainer
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width / 2
                    height: profilePictureLoader.height
                    property var thumbnailModel: ({})
                    property bool thumbnailVisible: true
                    property bool thumbnailActive: thumbnailModel.length > 0
                    property int thumbnailRadius: imageContainer.width / 2

                    Loader {
                        id: profilePictureLoader
                        active: imageContainer.thumbnailActive
                        asynchronous: true
                        width: Theme.itemSizeExtraLarge
                        height: Theme.itemSizeExtraLarge
                        anchors.horizontalCenter: parent.horizontalCenter
                        source: "../components/ProfilePictureList.qml"
                    }
                }

                Column {
                    spacing: Theme.paddingSmall
                    width: parent.width / 2

                    Button {
                        id: addProfilePictureButton
                        text: qsTr("Add Picture")
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                        onClicked: {

                        }
                    }

                    Button {
                        id: removeProfilePictureButton
                        text: qsTr("Remove Picture")
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                        onClicked: {

                        }
                    }

                }
            }

            SectionHeader {
                horizontalAlignment: Text.AlignLeft
                text: qsTr("Privacy Options")
            }

            Grid {
                width: parent.width
                columns: landscapeLayout ? 2 : 1
                columnSpacing: Theme.horizontalPageMargin
                anchors.horizontalCenter: parent.horizontalCenter

                readonly property real columnWidth: width/columns

                Connections {
                    target: tdLibWrapper
                    onUserPrivacySettingUpdated: {
                        Debug.log("Received updated privacy setting: " + setting + ":" + rule);
                        switch (setting) {
                        case TelegramAPI.SettingAllowChatInvites:
                            allowChatInvitesComboBox.currentIndex = rule;
                            break;
                        case TelegramAPI.SettingAllowFindingByPhoneNumber:
                            allowFindingByPhoneNumberComboBox.currentIndex = rule;
                            break;
                        case TelegramAPI.SettingShowLinkInForwardedMessages:
                            showLinkInForwardedMessagesComboBox.currentIndex = rule;
                            break;
                        case TelegramAPI.SettingShowPhoneNumber:
                            showPhoneNumberComboBox.currentIndex = rule;
                            break;
                        case TelegramAPI.SettingShowProfilePhoto:
                            showProfilePhotoComboBox.currentIndex = rule;
                            break;
                        case TelegramAPI.SettingShowStatus:
                            showStatusComboBox.currentIndex = rule;
                            break;
                        }
                    }
                }

                ComboBox {
                    id: allowChatInvitesComboBox
                    width: parent.columnWidth
                    label: qsTr("Allow chat invites")
                    description: qsTr("Privacy setting for managing whether you can be invited to chats.")
                    menu: ContextMenu {

                        MenuItem {
                            text: qsTr("Yes")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingAllowChatInvites, TelegramAPI.RuleAllowAll);
                            }
                        }
                        MenuItem {
                            text: qsTr("Your contacts only")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingAllowChatInvites, TelegramAPI.RuleAllowContacts);
                            }
                        }
                        MenuItem {
                            text: qsTr("No")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingAllowChatInvites, TelegramAPI.RuleRestrictAll);
                            }
                        }
                    }

                    Component.onCompleted: {
                        currentIndex = tdLibWrapper.getUserPrivacySettingRule(TelegramAPI.SettingAllowChatInvites);
                    }
                }

                ComboBox {
                    id: allowFindingByPhoneNumberComboBox
                    width: parent.columnWidth
                    label: qsTr("Allow finding by phone number")
                    description: qsTr("Privacy setting for managing whether you can be found by your phone number.")
                    menu: ContextMenu {

                        MenuItem {
                            text: qsTr("Yes")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingAllowFindingByPhoneNumber, TelegramAPI.RuleAllowAll);
                            }
                        }
                        MenuItem {
                            text: qsTr("Your contacts only")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingAllowFindingByPhoneNumber, TelegramAPI.RuleAllowContacts);
                            }
                        }
                    }

                    Component.onCompleted: {
                        currentIndex = tdLibWrapper.getUserPrivacySettingRule(TelegramAPI.SettingAllowFindingByPhoneNumber);
                    }
                }

                ComboBox {
                    id: showLinkInForwardedMessagesComboBox
                    width: parent.columnWidth
                    label: qsTr("Show link in forwarded messages")
                    description: qsTr("Privacy setting for managing whether a link to your account is included in forwarded messages.")
                    menu: ContextMenu {

                        MenuItem {
                            text: qsTr("Yes")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowLinkInForwardedMessages, TelegramAPI.RuleAllowAll);
                            }
                        }
                        MenuItem {
                            text: qsTr("Your contacts only")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowLinkInForwardedMessages, TelegramAPI.RuleAllowContacts);
                            }
                        }
                        MenuItem {
                            text: qsTr("No")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowLinkInForwardedMessages, TelegramAPI.RuleRestrictAll);
                            }
                        }
                    }

                    Component.onCompleted: {
                        currentIndex = tdLibWrapper.getUserPrivacySettingRule(TelegramAPI.SettingShowLinkInForwardedMessages);
                    }
                }

                ComboBox {
                    id: showPhoneNumberComboBox
                    width: parent.columnWidth
                    label: qsTr("Show phone number")
                    description: qsTr("Privacy setting for managing whether your phone number is visible.")
                    menu: ContextMenu {

                        MenuItem {
                            text: qsTr("Yes")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowPhoneNumber, TelegramAPI.RuleAllowAll);
                            }
                        }
                        MenuItem {
                            text: qsTr("Your contacts only")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowPhoneNumber, TelegramAPI.RuleAllowContacts);
                            }
                        }
                        MenuItem {
                            text: qsTr("No")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowPhoneNumber, TelegramAPI.RuleRestrictAll);
                            }
                        }
                    }

                    Component.onCompleted: {
                        currentIndex = tdLibWrapper.getUserPrivacySettingRule(TelegramAPI.SettingShowPhoneNumber);
                    }
                }

                ComboBox {
                    id: showProfilePhotoComboBox
                    width: parent.columnWidth
                    label: qsTr("Show profile photo")
                    description: qsTr("Privacy setting for managing whether your profile photo is visible.")
                    menu: ContextMenu {

                        MenuItem {
                            text: qsTr("Yes")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowProfilePhoto, TelegramAPI.RuleAllowAll);
                            }
                        }
                        MenuItem {
                            text: qsTr("Your contacts only")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowProfilePhoto, TelegramAPI.RuleAllowContacts);
                            }
                        }
                        MenuItem {
                            text: qsTr("No")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowProfilePhoto, TelegramAPI.RuleRestrictAll);
                            }
                        }
                    }

                    Component.onCompleted: {
                        currentIndex = tdLibWrapper.getUserPrivacySettingRule(TelegramAPI.SettingShowProfilePhoto);
                    }
                }

                ComboBox {
                    id: showStatusComboBox
                    width: parent.columnWidth
                    label: qsTr("Show status")
                    description: qsTr("Privacy setting for managing whether your online status is visible.")
                    menu: ContextMenu {

                        MenuItem {
                            text: qsTr("Yes")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowStatus, TelegramAPI.RuleAllowAll);
                            }
                        }
                        MenuItem {
                            text: qsTr("Your contacts only")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowStatus, TelegramAPI.RuleAllowContacts);
                            }
                        }
                        MenuItem {
                            text: qsTr("No")
                            onClicked: {
                                tdLibWrapper.setUserPrivacySettingRule(TelegramAPI.SettingShowStatus, TelegramAPI.RuleRestrictAll);
                            }
                        }
                    }

                    Component.onCompleted: {
                        currentIndex = tdLibWrapper.getUserPrivacySettingRule(TelegramAPI.SettingShowStatus);
                    }
                }

            }

            SectionHeader {
                text: qsTr("Behavior")
            }

            Grid {
                width: parent.width
                columns: landscapeLayout ? 2 : 1

                readonly property real columnWidth: width/columns

                TextSwitch {
                    width: parent.columnWidth
                    checked: appSettings.sendByEnter
                    text: qsTr("Send message by enter")
                    description: qsTr("Send your message by pressing the enter key")
                    automaticCheck: false
                    onClicked: {
                        appSettings.sendByEnter = !checked
                    }
                }

                TextSwitch {
                    width: parent.columnWidth
                    checked: appSettings.focusTextAreaAfterSend
                    text: qsTr("Focus text input area after send")
                    description: qsTr("Focus the text input area after sending a message")
                    automaticCheck: false
                    onClicked: {
                        appSettings.focusTextAreaAfterSend = !checked
                    }
                }

                TextSwitch {
                    width: parent.columnWidth
                    checked: appSettings.useOpenWith
                    text: qsTr("Open-with menu integration")
                    description: qsTr("Integrate Fernschreiber into open-with menu of Sailfish OS")
                    automaticCheck: false
                    onClicked: {
                        appSettings.useOpenWith = !checked
                    }
                }

                TextSwitch {
                    width: parent.columnWidth
                    checked: appSettings.notificationTurnsDisplayOn && enabled
                    text: qsTr("Notification turns on the display")
                    height: appSettings.notificationFeedback === AppSettings.NotificationFeedbackNone ? 0 : implicitHeight
                    clip: height < implicitHeight
                    visible: height > 0
                    automaticCheck: false
                    onClicked: {
                        appSettings.notificationTurnsDisplayOn = !checked
                    }
                    Behavior on height { SmoothedAnimation { duration: 200 } }
                }
            }

            ComboBox {
                id: feedbackComboBox
                label: qsTr("Notification feedback")
                description: qsTr("Use non-graphical feedback (sound, vibration) for notifications")
                menu: ContextMenu {
                    id: feedbackMenu

                    MenuItem {
                        readonly property int value: AppSettings.NotificationFeedbackAll
                        text: qsTr("All events")
                        onClicked: {
                            appSettings.notificationFeedback = value
                        }
                    }
                    MenuItem {
                        readonly property int value: AppSettings.NotificationFeedbackNew
                        text: qsTr("Only new events")
                        onClicked: {
                            appSettings.notificationFeedback = value
                        }
                    }
                    MenuItem {
                        readonly property int value: AppSettings.NotificationFeedbackNone
                        text: qsTr("None")
                        onClicked: {
                            appSettings.notificationFeedback = value
                        }
                    }
                }

                Component.onCompleted: updateFeedbackSelection()

                function updateFeedbackSelection() {
                    var menuItems = feedbackMenu.children
                    var n = menuItems.length
                    for (var i=0; i<n; i++) {
                        if (menuItems[i].value === appSettings.notificationFeedback) {
                            currentIndex = i
                            return
                        }
                    }
                }

                Connections {
                    target: appSettings
                    onNotificationFeedbackChanged: {
                        feedbackComboBox.updateFeedbackSelection()
                    }
                }
            }

            SectionHeader {
                text: qsTr("Appearance")
            }

            Grid {
                width: parent.width
                columns: landscapeLayout ? 2 : 1

                readonly property real columnWidth: width/columns

                TextSwitch {
                    width: parent.columnWidth
                    checked: appSettings.showStickersAsImages
                    text: qsTr("Show stickers as images")
                    description: qsTr("Show background for stickers and align them centrally like images")
                    automaticCheck: false
                    onClicked: {
                        appSettings.showStickersAsImages = !checked
                    }
                }

                TextSwitch {
                    width: parent.columnWidth
                    checked: appSettings.animateStickers
                    text: qsTr("Animate stickers")
                    automaticCheck: false
                    onClicked: {
                        appSettings.animateStickers = !checked
                    }
                }
            }

            SectionHeader {
                text: qsTr("Privacy")
            }

            TextSwitch {
                checked: appSettings.allowInlineBotLocationAccess
                text: qsTr("Allow sending Location to inline bots")
                description: qsTr("Some inline bots request location data when using them")
                automaticCheck: false
                onClicked: {
                    appSettings.allowInlineBotLocationAccess = !checked
                }
            }

            SectionHeader {
                text: qsTr("Storage")
            }

            Grid {
                width: parent.width
                columns: landscapeLayout ? 2 : 1

                readonly property real columnWidth: width/columns

                TextSwitch {
                    width: parent.columnWidth
                    checked: appSettings.onlineOnlyMode
                    text: qsTr("Enable online-only mode")
                    description: qsTr("Disables offline caching. Certain features may be limited or missing in this mode. Changes require a restart of Fernschreiber to take effect.")
                    automaticCheck: false
                    onClicked: {
                        appSettings.onlineOnlyMode = !checked
                    }
                }

                TextSwitch {
                    width: parent.columnWidth
                    checked: appSettings.storageOptimizer
                    text: qsTr("Enable storage optimizer")
                    automaticCheck: false
                    onClicked: {
                        appSettings.storageOptimizer = !checked
                    }
                }
            }

            Item {
                width: 1
                height: Theme.paddingLarge // Some space at the bottom
            }
        }

        VerticalScrollDecorator {}
    }
}
