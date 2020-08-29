/*
    Copyright (C) 2020 Sebastian J. Wolf

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
import QtQuick 2.2
import Sailfish.Silica 1.0
import WerkWolf.Fernschreiber 1.0

Page {
    id: initializationPage
    allowedOrientations: Orientation.All
    backNavigation: false

    property bool loading: true
    property int authorizationState: TelegramAPI.Closed

    BusyLabel {
        text: qsTr("Loading...")
        running: initializationPage.loading
    }

    Component.onCompleted: {
        initializationPage.authorizationState = tdLibWrapper.getAuthorizationState();
        initializationPage.loading = false;
        if (initializationPage.authorizationState === TelegramAPI.WaitCode) {
            welcomeFlickable.visible = false;
            enterPinColumn.visible = true;
        }
    }

    Connections {
        target: tdLibWrapper
        onAuthorizationStateChanged: {
            switch (authorizationState) {
            case TelegramAPI.WaitCode:
                initializationPage.loading = false;
                enterPinColumn.visible = true;
                break;
            case TelegramAPI.AuthorizationReady:
                initializationPage.loading = false;
                pageStack.completeAnimation();
                pageStack.pop();
                break;
            default:
                // Nothing ;)
            }
        }
    }

    Column {
        y: ( parent.height - ( errorInfoLabel.height + fernschreiberErrorImage.height + errorOkButton.height + ( 3 * Theme.paddingSmall ) ) ) / 2
        width: parent.width
        id: pinErrorColumn
        spacing: Theme.paddingSmall

        Behavior on opacity { NumberAnimation {} }
        opacity: visible ? 1 : 0
        visible: false

        Image {
            id: fernschreiberErrorImage
            source: "../../images/fernschreiber.png"
            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            fillMode: Image.PreserveAspectFit
            asynchronous: true
            width: 1/2 * parent.width
        }

        InfoLabel {
            id: errorInfoLabel
            font.pixelSize: Theme.fontSizeLarge
            text: ""
        }

        Button {
            id: errorOkButton
            text: qsTr("OK")
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            onClicked: {
                pinErrorColumn.visible = false;
                welcomeFlickable.visible = true;
            }
        }
    }

    Column {
        y: ( parent.height - ( fernschreiberPinImage.height + enterPinLabel.height + enterPinField.height + enterPinButton.height + ( 3 * Theme.paddingSmall ) ) ) / 2
        width: parent.width
        id: enterPinColumn
        spacing: Theme.paddingSmall

        Behavior on opacity { NumberAnimation {} }
        opacity: visible ? true : false
        visible: false

        Image {
            id: fernschreiberPinImage
            source: "../../images/fernschreiber.png"
            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            fillMode: Image.PreserveAspectFit
            asynchronous: true
            width: 1/2 * parent.width
        }

        InfoLabel {
            id: enterPinLabel
            font.pixelSize: Theme.fontSizeLarge
            text: qsTr("Please enter the code that you received:")
        }

        TextField {
            id: enterPinField
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            inputMethodHints: Qt.ImhDigitsOnly
            font.pixelSize: Theme.fontSizeExtraLarge
            width: parent.width - 4 * Theme.paddingLarge
            horizontalAlignment: TextInput.AlignHCenter
        }

        Button {
            id: enterPinButton
            text: qsTr("OK")
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            onClicked: {
                initializationPage.loading = true;
                enterPinColumn.visible = false;
                tdLibWrapper.setAuthenticationCode(enterPinField.text);
            }
        }
    }

    Column {
        y: ( parent.height - ( fernschreiberLinkingErrorImage.height + linkingErrorInfoLabel.height + errorOkButton.height + ( 3 * Theme.paddingSmall ) ) ) / 2
        width: parent.width
        id: linkingErrorColumn
        spacing: Theme.paddingSmall

        Behavior on opacity { NumberAnimation {} }
        opacity: visible ? true : false
        visible: false

        Image {
            id: fernschreiberLinkingErrorImage
            source: "../../images/fernschreiber.png"
            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            fillMode: Image.PreserveAspectFit
            asynchronous: true
            width: 1/2 * parent.width
        }

        InfoLabel {
            id: linkingErrorInfoLabel
            font.pixelSize: Theme.fontSizeLarge
            text: qsTr("Unable to authenticate you with the entered code.")
        }

        Button {
            id: enterPinAgainButton
            text: qsTr("Enter code again")
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            onClicked: {
                linkingErrorColumn.visible = false;
                enterPinColumn.visible = true;
            }
        }

        Button {
            id: restartAuthenticationButton
            text: qsTr("Restart authentication")
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            onClicked: {
                linkingErrorColumn.visible = false;
                welcomeFlickable.visible = true;
            }
        }
    }

    SilicaFlickable {
        id: welcomeFlickable
        contentHeight: welcomeColumn.height
        Behavior on opacity { NumberAnimation {} }
        anchors.fill: parent
        opacity: visible ? 1 : 0

        Column {
            id: welcomeColumn
            width: parent.width
            spacing: Theme.paddingSmall

            PageHeader {
                title: qsTr("Welcome to Fernschreiber!")
            }

            Image {
                id: fernschreiberImage
                source: "../../images/fernschreiber.png"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                asynchronous: true
                width: 1/2 * parent.width
            }

            Label {
                id: enterPhoneNumberLabel
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Please enter your phone number to continue.")
                font.pixelSize: Theme.fontSizeMedium
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            TextField {
                id: phoneNumberTextField
                placeholderText: "Use the international format, e.g. +4912342424242"
                inputMethodHints: Qt.ImhDialableCharactersOnly
                labelVisible: false
                width: parent.width
            }

            Button {
                id: continueWithPhoneNumberButton
                text: qsTr("Continue")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                enabled: phoneNumberTextField.text.match(/\+[1-9][0-9]{4,}/g)
                onClicked: {
                    initializationPage.loading = true;
                    welcomeFlickable.visible = false;
                    tdLibWrapper.setAuthenticationPhoneNumber(phoneNumberTextField.text);
                }
            }

        }
    }

}

