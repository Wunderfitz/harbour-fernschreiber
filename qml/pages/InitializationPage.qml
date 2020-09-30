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
import QtQuick 2.6
import Sailfish.Silica 1.0
import WerkWolf.Fernschreiber 1.0

Page {
    id: initializationPage
    allowedOrientations: Orientation.All
    backNavigation: false

    property bool loading: true
    property int authorizationState: TelegramAPI.Closed
    property var authorizationStateData: null

    BusyLabel {
        text: qsTr("Loading...")
        running: initializationPage.loading
    }

    Component.onCompleted: {
        initializationPage.authorizationState = tdLibWrapper.getAuthorizationState();
        initializationPage.authorizationStateData = tdLibWrapper.getAuthorizationStateData();
        initializationPage.loading = false;

        switch (authorizationState) {
        case TelegramAPI.WaitCode:
            initializationPage.loading = false;
            welcomeColumn.visible = false;
            enterPinColumn.visible = true;
            enterPasswordColumn.visible = false;
            waitRegistrationColumn.visible = false;
            break;
        case TelegramAPI.WaitPassword:
            initializationPage.loading = false;
            welcomeColumn.visible = false;
            enterPinColumn.visible = false;
            enterPasswordColumn.visible = true;
            waitRegistrationColumn.visible = false;
            break;
        case TelegramAPI.WaitRegistration:
            initializationPage.loading = false;
            welcomeColumn.visible = false;
            enterPinColumn.visible = false;
            enterPasswordColumn.visible = false;
            waitRegistrationColumn.visible = true;
            break;
        default:
            // Nothing ;)
        }
    }

    Connections {
        target: tdLibWrapper
        onAuthorizationStateChanged: {
            switch (authorizationState) {
            case TelegramAPI.WaitCode:
                initializationPage.loading = false;
                enterPinColumn.visible = true;
                enterPasswordColumn.visible = false;
                waitRegistrationColumn.visible = false;
                break;
            case TelegramAPI.WaitPassword:
                initializationPage.loading = false;
                enterPinColumn.visible = false;
                enterPasswordColumn.visible = true;
                waitRegistrationColumn.visible = false;
                break;
            case TelegramAPI.WaitRegistration:
                initializationPage.loading = false;
                enterPinColumn.visible = false;
                enterPasswordColumn.visible = false;
                waitRegistrationColumn.visible = true;
                break;
            case TelegramAPI.AuthorizationReady:
                initializationPage.loading = false;
                pageStack.completeAnimation();
                pageStack.pop();
                break;
            default:
                // Nothing ;)
            }
            initializationPage.authorizationStateData = authorizationStateData;
        }
    }


    SilicaFlickable {
        id: welcomeFlickable
        contentHeight: contentItem.childrenRect.height
        Behavior on contentHeight { NumberAnimation {} }
        anchors.fill: parent

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
                    welcomeColumn.visible = false;
                    tdLibWrapper.setAuthenticationPhoneNumber(phoneNumberTextField.text);
                }
            }

        }

        Column {
            id: pinErrorColumn
//            y: ( parent.height - ( errorInfoLabel.height + fernschreiberErrorImage.height + errorOkButton.height + ( 3 * Theme.paddingSmall ) ) ) / 2
            width: parent.width
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
                    welcomeColumn.visible = true;
                }
            }
        }

        Column {
            id: enterPasswordColumn
//            y: ( parent.height - ( fernschreiberPasswordImage.height + enterPasswordLabel.height + enterPasswordField.height + enterPasswordButton.height + ( 3 * Theme.paddingSmall ) ) ) / 2
            width: parent.width
            spacing: Theme.paddingSmall

            Behavior on opacity { NumberAnimation {} }
            opacity: visible ? 1.0 : 0.0
            visible: false

            Image {
                id: fernschreiberPasswordImage
                source: "../../images/fernschreiber.png"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                asynchronous: true
                width: 1/2 * parent.width
            }

            InfoLabel {
                id: enterPasswordLabel
                font.pixelSize: Theme.fontSizeLarge
                text: qsTr("Please enter your password:")
            }

            PasswordField {
                id: enterPasswordField
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeLarge
                width: parent.width - 2 * Theme.horizontalPageMargin
                horizontalAlignment: TextInput.AlignHCenter
            }

            Button {
                id: enterPasswordButton
                text: qsTr("OK")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    initializationPage.loading = true;
                    enterPasswordColumn.visible = false;
                    tdLibWrapper.setAuthenticationPassword(enterPasswordField.text);
                }
            }
        }

        Column {
            id: enterPinColumn
            topPadding: Theme.paddingLarge
            bottomPadding: Theme.paddingLarge
            width: parent.width
            spacing: Theme.paddingSmall

            Behavior on opacity { NumberAnimation {} }
            opacity: visible ? 1.0 : 0.0
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
            id: linkingErrorColumn
            topPadding: Theme.paddingLarge
            bottomPadding: Theme.paddingLarge
            width: parent.width
            spacing: Theme.paddingSmall

            Behavior on opacity { NumberAnimation {} }
            opacity: visible ? 1.0 : 0.0
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
                    welcomeColumn.visible = true;
                }
            }
        }

        Column {
            id: waitRegistrationColumn
            topPadding: Theme.paddingLarge
            bottomPadding: Theme.paddingLarge
            width: parent.width
            spacing: Theme.paddingLarge

            Behavior on opacity { NumberAnimation {} }
            opacity: visible ? 1.0 : 0.0
            visible: false

            PageHeader {
                title: qsTr("User Registration")
            }
            Image {
                id: waitRegistrationImage
                source: "../../images/fernschreiber.png"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                asynchronous: true
                width:  1/2 * Screen.width
            }

            InfoLabel {
                id: waitRegistrationInfoLabel
                property bool acknowledged
                font.pixelSize: Theme.fontSizeExtraSmall
                textFormat: Text.StyledText
                horizontalAlignment: Text.AlignLeft
                linkColor: Theme.primaryColor
                property var stateText: initializationPage.authorizationStateData.authorization_state && initializationPage.authorizationStateData.authorization_state.terms_of_service ?
                                               initializationPage.authorizationStateData.authorization_state.terms_of_service.text :
                                               null
                visible: !!stateText && !acknowledged
                text: {
                    if(!stateText) {
                        return '';
                    }
                    var entities = stateText.entities;
                    if(entities && entities.length > 0 && entities[0]["type"]["@type"] === "textEntityTypeTextUrl") { //we just use the first entity for now.
                        var offset = entities[0].offset;
                        var length = entities[0].length;
                        return (stateText.text.slice(0,entities[0].offset)
                                + "<a href=\""+entities[0]["type"]["url"]+"\">"
                                + stateText.text.slice(entities[0].offset, entities[0].offset + entities[0].length)
                                + '</a>'
                                + stateText.text.slice(entities[0].offset + entities[0].length)).replace(/\n/gm, "<br>");
                    }
                    return stateText.text.replace(/\n/gm, "<br>");


                }
                    //JSON.stringify(initializationPage.authorizationStateData, null, 2)//qsTr("Unable to authenticate you with the entered code.")
            }

            Button {
                id: acknowledgeTOCButton
                visible: waitRegistrationInfoLabel.visible
                text: qsTr("OK")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    waitRegistrationInfoLabel.acknowledged = true;
                    userFirstNameTextField.focus = true
                }
            }
            RegExpValidator {
                id: filledValidator
                regExp: /.+/
            }
            TextField {
                id: userFirstNameTextField
                visible: !waitRegistrationInfoLabel.visible
                opacity: visible ? 1.0 : 0.0
                Behavior on opacity { NumberAnimation {} }
                placeholderText: qsTr("Enter your First Name")
                labelVisible: false
                width: parent.width
                EnterKey.iconSource: !!text ? "image://theme/icon-m-enter-next" : "image://theme/icon-m-text-input"
                EnterKey.onClicked: {
                    validator = filledValidator
                    if(acceptableInput) userLastNameTextField.focus = true;
                }
            }
            TextField {
                id: userLastNameTextField
                visible: !waitRegistrationInfoLabel.visible
                opacity: visible ? 1.0 : 0.0
                Behavior on opacity { NumberAnimation {} }
                placeholderText: qsTr("Enter your Last Name")
                labelVisible: false
                width: parent.width
                EnterKey.iconSource: !!text ? "image://theme/icon-m-enter-accept" : "image://theme/icon-m-text-input"
                EnterKey.onClicked: {
                    validator = filledValidator
                    if(acceptableInput) registerUserButton.onClicked(null);
                }
            }
            Button {
                id: registerUserButton
                visible: !waitRegistrationInfoLabel.visible
                opacity: visible ? 1.0 : 0.0
                Behavior on opacity { NumberAnimation {} }
                text: qsTr("Register User")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    userFirstNameTextField.validator = filledValidator
                    userLastNameTextField.validator = filledValidator
                    if(userFirstNameTextField.acceptableInput && userLastNameTextField.acceptableInput) {
                        tdLibWrapper.registerUser(userFirstNameTextField.text, userLastNameTextField.text);
                    } else if(!userFirstNameTextField.acceptableInput) {
                        userFirstNameTextField.focus = true;
                    } else {
                        userLastNameTextField.focus = true;
                    }

                }
            }
        }
    }

}

