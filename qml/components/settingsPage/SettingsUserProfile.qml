/*
    Copyright (C) 2021 Sebastian J. Wolf and other contributors

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
import Sailfish.Pickers 1.0
import WerkWolf.Fernschreiber 1.0
import "../"
import "../../pages/"
import "../../js/twemoji.js" as Emoji
import "../../js/functions.js" as Functions

AccordionItem {
    text: qsTr("User Profile")
    Component {
        Column {
            id: accordionContent
            bottomPadding: Theme.paddingMedium

            readonly property var userInformation: tdLibWrapper.getUserInformation()
            property bool uploadInProgress: false
            property variant activeSessions;
            property bool loaded : false;

            Component.onCompleted: {
                tdLibWrapper.getUserProfilePhotos(userInformation.id, 100, 0);
                if (!activeSessions) {
                    tdLibWrapper.getActiveSessions();
                } else {
                    accordionContent.loaded = true;
                }
            }

            Connections {
                target: tdLibWrapper
                onSessionsReceived: {
                    accordionContent.activeSessions = sessions;
                    accordionContent.loaded = true;
                }
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
                onFileUpdated: {
                    if (uploadInProgress) {
                        profilePictureButtonColumn.visible = !fileInformation.remote.is_uploading_active;
                        uploadInProgress = fileInformation.remote.is_uploading_active;
                        if (!fileInformation.remote.is_uploading_active) {
                            uploadInProgress = false;
                            tdLibWrapper.getUserProfilePhotos(userInformation.id, 100, 0);
                        }
                    }
                }
                onOkReceived: {
                    if (request === "terminateSession") {
                        appNotification.show(qsTr("Session was terminated"));
                        accordionContent.loaded = false;
                        tdLibWrapper.getActiveSessions();
                    }
                    if (request === "deleteProfilePhoto") {
                        tdLibWrapper.getUserProfilePhotos(userInformation.id, 100, 0);
                    }
                    if (request === "setProfilePhoto") {
                        tdLibWrapper.getUserProfilePhotos(userInformation.id, 100, 0);
                        profilePictureButtonColumn.visible = true;
                        uploadInProgress = false;
                    }
                }
            }

            ResponsiveGrid {
                x: Theme.horizontalPageMargin

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
                        source: "../ProfilePictureList.qml"
                    }

                    ProfileThumbnail {
                        id: chatPictureReplacement
                        visible: !profilePictureLoader.active
                        replacementStringHint: Functions.getUserName(accordionContent.userInformation)
                        radius: imageContainer.thumbnailRadius
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: Theme.itemSizeExtraLarge
                        height: Theme.itemSizeExtraLarge
                    }
                }

                Column {
                    id: profilePictureButtonColumn
                    spacing: Theme.paddingSmall
                    width: parent.width / 2

                    Button {
                        id: addProfilePictureButton
                        text: qsTr("Add Picture")
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                        onClicked: {
                            pageStack.push(imagePickerPage);
                        }
                    }

                    Button {
                        id: removeProfilePictureButton
                        text: qsTr("Delete Picture")
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                        onClicked: {
                            var pictureIdForDeletion = imageContainer.thumbnailModel[profilePictureLoader.item.currentPictureIndex].id;
                            Remorse.popupAction(settingsPage, qsTr("Deleting profile picture"), function() { tdLibWrapper.deleteProfilePhoto(pictureIdForDeletion) });
                        }
                    }
                }

                Column {
                    id: uploadStatusColumn
                    visible: !profilePictureButtonColumn.visible
                    spacing: Theme.paddingMedium
                    width: parent.width / 2

                    Text {
                        id: uploadingText
                        font.pixelSize: Theme.fontSizeSmall
                        text: qsTr("Uploading...")
                        horizontalAlignment: Text.AlignHCenter
                        color: Theme.secondaryColor
                        width: parent.width
                    }

                    BusyIndicator {
                        anchors.horizontalCenter: parent.horizontalCenter
                        running: uploadStatusColumn.visible
                        size: BusyIndicatorSize.Medium
                    }

                }

            }

            Component {
                id: imagePickerPage
                ImagePickerPage {
                    onSelectedContentPropertiesChanged: {
                        profilePictureButtonColumn.visible = false;
                        uploadInProgress = true;
                        tdLibWrapper.setProfilePhoto(selectedContentProperties.filePath);
                    }
                }
            }

            SectionHeader {
                horizontalAlignment: Text.AlignLeft
                text: qsTr("Logged in as")
            }

            Row {
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                spacing: Theme.paddingMedium

                Loader {
                    id: userInformationLoader
                    active: tdLibWrapper.authorizationState === TelegramAPI.AuthorizationReady
                    width: parent.width / 2
                    sourceComponent: Component {
                        Column {
                            width: parent.width
                            spacing: Theme.paddingMedium
                            anchors.topMargin: Theme.paddingMedium

                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                horizontalAlignment: Text.AlignHCenter
                                text: Emoji.emojify(accordionContent.userInformation.first_name + " " + accordionContent.userInformation.last_name, Theme.fontSizeSmall)
                                font.pixelSize: Theme.fontSizeSmall
                                wrapMode: Text.Wrap
                                color: Theme.primaryColor
                                textFormat: Text.StyledText
                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                }
                            }

                            ProfileThumbnail {
                                photoData: ((typeof accordionContent.userInformation.profile_photo !== "undefined") ? accordionContent.userInformation.profile_photo.small : {})
                                anchors.horizontalCenter: parent.horizontalCenter
                                width: Theme.itemSizeSmall
                                height: Theme.itemSizeSmall
                                replacementStringHint: accordionContent.userInformation.first_name + " " + accordionContent.userInformation.last_name
                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                }
                            }

                            Label {
                                width: accordionContent.width / 2 - Theme.horizontalPageMargin
                                anchors.horizontalCenter: parent.horizontalCenter
                                horizontalAlignment: Text.AlignHCenter
                                text: qsTr("Phone number: +%1").arg(accordionContent.userInformation.phone_number)
                                font.pixelSize: Theme.fontSizeSmall
                                wrapMode: Text.Wrap
                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                }
                            }
                        }
                    }
                }
                BackgroundItem {
                    id: logOutItem
                    width: parent.width / 2
                    height: userInformationLoader.height
                    anchors.verticalCenter: parent.verticalCenter
                    enabled: userInformationLoader.status == Loader.Ready
                    function showRemorseItem() {
                        remorse.execute(logOutItem, qsTr("Logged out"), function() {
                            tdLibWrapper.logout();
                            pageStack.pop();
                        });
                    }
                    RemorseItem { id: remorse }
                    Button {
                        id: logOutButton
                        text: qsTr("Log Out")
                        anchors.centerIn: parent
                        onClicked: logOutItem.showRemorseItem()
                    }
                }
            }
        }
    }
}
