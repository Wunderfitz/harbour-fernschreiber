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
import "../../js/functions.js" as Functions

AccordionItem {
    text: qsTr("User Profile")
    Component {
        Column {
            id: accordionContent
            bottomPadding: Theme.paddingMedium

            readonly property var userInformation: tdLibWrapper.getUserInformation()
            property bool uploadInProgress: false
            property bool contactSyncEnabled: false

            Component.onCompleted: {
                tdLibWrapper.getUserProfilePhotos(userInformation.id, 100, 0);
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

            Column {
                id: contactSyncItem
                width: parent.width
                height: syncInProgress ? ( syncContactsBusyIndicator.height + Theme.paddingMedium ) : ( syncContactsButton.height + Theme.paddingMedium )
                visible: accordionContent.contactSyncEnabled

                property bool syncInProgress: false

                Connections {
                    target: contactSyncLoader.item
                    onSyncError: {
                        contactSyncItem.syncInProgress = false;
                    }
                }

                Connections {
                    target: tdLibWrapper
                    onContactsImported: {
                        appNotification.show(qsTr("Contacts successfully synchronized with Telegram."));
                    }
                }

                Button {
                    id: syncContactsButton
                    text: qsTr("Synchronize Contacts with Telegram")
                    visible: !contactSyncItem.syncInProgress
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }
                    onClicked: {
                        contactSyncLoader.item.synchronize();
                    }
                }

                BusyIndicator {
                    id: syncContactsBusyIndicator
                    anchors.horizontalCenter: parent.horizontalCenter
                    running: contactSyncItem.syncInProgress
                    size: BusyIndicatorSize.Small
                    visible: running
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

            Loader {
                id: contactSyncLoader
                source: "../ContactSync.qml"
                active: true
                onLoaded: {
                    accordionContent.contactSyncEnabled = true;
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
        }
    }
}
