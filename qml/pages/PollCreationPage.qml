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
import QtMultimedia 5.0
import "../components"
import "../js/functions.js" as Functions
import "../js/twemoji.js" as Emoji

Dialog {
    id: pollCreationPage
    allowedOrientations: Orientation.All
    property string groupName
    // poll request data start
    property string chatId
    property alias pollQuestion: questionTextArea.text
    property ListModel options: ListModel {
        ListElement {
            text: ""
        }
    }
    property alias anonymous: anonymousSwitch.checked
    property int correctOption: -1
    property alias quiz: quizSwitch.checked
    property alias multiple: multipleSwitch.checked
    property string replyToMessageId: "0"
    // poll request data end

    canAccept: validationErrors.length === 0
    onDone: {
    }
    onAcceptPendingChanged: {
        if(acceptPending) {

            validate();

            if(validationErrors.length > 0) {
                validationErrorsVisible = true;
                contentFlickable.scrollToTop();
            }
        }
    }

    property var validationErrorsVisible: false
    property var validationErrors:[""]

    function validate() {
        var errors = [];
        if(pollQuestion.length === 0) {
            errors.push(qsTr("You have to enter a question."));
        } else if(pollQuestion.length > 255) {
            errors.push(qsTr("The question has to be shorter than 256 characters."));
        }

        if(options.count < 2 || options.count > 10) {
            errors.push(qsTr("A poll requires 2-10 answers."));
        } else {
            for(var i = 0; i < options.count; i += 1) {
                var len = options.get(i).text.length
                if(len < 1 || len > 100) {
                    errors.push(qsTr("All answers have to contain 1-100 characters."));
                    break;
                }
            }
        }
        if(quiz && (correctOption < 0 || correctOption > options.count - 1)) {
            errors.push(qsTr("To send a quiz, you have to specify the right answer."));
        }
        if(errors.length === 0) {
            validationErrorsVisible = false;
        }

        validationErrors = errors;
    }
    function createNewOption() {
        if(options.count < 10) {
            pollCreationPage.options.append({text:""});
            focusLastOptionTimer.start();
        }
    }

    signal focusOption(int focusIndex)
    DialogHeader {
        id: header
        dialog: pollCreationPage
        title: qsTr("Create a Poll", "Dialog Header")
    }
    Label {
        id: subHeaderLabel
        anchors {
            verticalCenter: header.bottom
            left: parent.left
            right: parent.right
            leftMargin: Theme.horizontalPageMargin
            rightMargin: Theme.horizontalPageMargin
        }

        color: Theme.secondaryHighlightColor
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        text: qsTr("in %1", "After dialog header… Create a Poll in [group name]").arg(Emoji.emojify(pollCreationPage.groupName, font.pixelSize))
        font.pixelSize: Theme.fontSizeSmall
    }

    SilicaFlickable {
        id: contentFlickable
        clip: true
        anchors {
            top: subHeaderLabel.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        contentHeight: contentColumn.height

        Column {
            id: contentColumn
            width: parent.width
            topPadding: Theme.paddingLarge
            bottomPadding: Theme.paddingLarge

            Item {
                id: errorItem
                width: parent.width - Theme.horizontalPageMargin * 2
                x: Theme.horizontalPageMargin
                property bool shown: pollCreationPage.validationErrorsVisible && pollCreationPage.validationErrors.length > 0
                property int visibleHeight: errorContentColumn.height
                height: pollCreationPage.validationErrorsVisible ? visibleHeight : 0
                clip: true;
                opacity: pollCreationPage.validationErrorsVisible ? 1.0 : 0.0
                Behavior on opacity { PropertyAnimation {duration: 500; easing.type: Easing.InOutCubic}}
                Behavior on height { PropertyAnimation {duration: 200; easing.type: Easing.InOutCubic}}
                Rectangle {
                    color: Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)
                    anchors.fill: parent
                    radius: Theme.paddingLarge
                    IconButton {
                        icon.source: "image://theme/icon-m-close"
                        anchors {
                            right: parent.right
                            top: parent.top
                        }
                        onClicked: {
                            pollCreationPage.validationErrorsVisible = false
                        }
                    }
                }


                Column {
                    id: errorContentColumn
                    width: parent.width - Theme.paddingLarge * 2 - Theme.itemSizeSmall
                    spacing: Theme.paddingMedium
                    padding: Theme.paddingLarge
                    Repeater {
                        model: pollCreationPage.validationErrors
                        delegate: Label {
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.highlightColor
                            width: errorContentColumn.width
                            text: modelData
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            leftPadding: Theme.iconSizeSmall + Theme.paddingSmall
                            Icon {
                                highlighted: true
                                source: "image://theme/icon-s-high-importance"
                                y: Theme.paddingSmall / 2
                            }
                        }
                    }

                }
            }

            TextArea {
                id: questionTextArea
                width: parent.width
                placeholderText: qsTr("Enter your question here")
                property int charactersLeft: 255 - text.length
                color: charactersLeft < 0 ? Theme.errorColor : Theme.highlightColor
                label: qsTr("Question (%n1 characters left)", "", charactersLeft).arg(charactersLeft)
                wrapMode: TextEdit.Wrap
                onFocusChanged: {
                    validate();
                }
            }
            SectionHeader {
                topPadding: 0
                text: qsTr("Answers", "Section header")
            }

            Column {
                id: optionsListView
                width: parent.width - Theme.horizontalPageMargin * 2
                x: Theme.horizontalPageMargin
                add: Transition {
                    NumberAnimation { properties: "opacity"; from: 0; to: 1; duration: 200; easing.type: Easing.InOutCubic }
                    NumberAnimation { properties: "height"; from: 0; to: ViewTransition.item.childrenRect.height; duration: 200; easing.type: Easing.InOutCubic }
                }
                move: Transition {
                    NumberAnimation { properties: "y"; duration: 200; easing.type: Easing.InOutCubic }
                }
                Behavior on height { PropertyAnimation {duration: 200; easing.type: Easing.InOutCubic}}
                Repeater {
                    model: pollCreationPage.options

                    delegate: Row {
                        width: parent.width
                        BackgroundItem {
                            id: answerCorrectBackgroundItem
                            width: enabled ? Theme.itemSizeSmall : 0
                            contentItem.radius: height/2
                            height: Theme.itemSizeSmall
                            property bool checked: pollCreationPage.correctOption === index
                            enabled: pollCreationPage.quiz
                            opacity: enabled ? (checked ? 1.0 : 0.5) : 0.0
                            Behavior on opacity { PropertyAnimation {duration: 500; easing.type: Easing.InOutCubic}}
                            Behavior on width { PropertyAnimation {duration: 500; easing.type: Easing.InOutCubic}}
                            Icon {
                                source: "image://theme/icon-m-accept"
                                anchors.centerIn: parent
                            }
                            onClicked: {
                                pollCreationPage.correctOption = index
                                validate();
                            }

                        }

                        TextField {
                            id: answerTextArea
                            textMargin: Theme.paddingSmall
                            width: answerCorrectBackgroundItem.enabled ? parent.width - Theme.itemSizeSmall * 2 : parent.width - Theme.itemSizeSmall
                            Behavior on width { PropertyAnimation {duration: 500; easing.type: Easing.InOutCubic}}
                            text: model.text
                            onTextChanged: {
                                pollCreationPage.options.setProperty(index, "text", text)
                                pollCreationPage.validate()
                            }
                            placeholderText: qsTr("Enter an answer here")
                            property int charactersLeft: 100 - text.length
                            color: charactersLeft < 0 ? Theme.errorColor : Theme.highlightColor
                            label: qsTr("Answer (%n1 characters left)", "", charactersLeft).arg(charactersLeft)
                            property bool hasNextOption: index < pollCreationPage.options.count - 1
                            EnterKey.onClicked: {
                                if(hasNextOption) {
                                    pollCreationPage.focusOption(index + 1);
                                } else if(pollCreationPage.options.count < 10) {
                                    pollCreationPage.createNewOption();
                                } else {
                                    focus = false;
                                }
                            }
                            EnterKey.iconSource: hasNextOption ? "image://theme/icon-m-enter-next" : (pollCreationPage.options.count < 10 ? "image://theme/icon-m-add" : "image://theme/icon-m-enter-close")

                            onFocusChanged: {
                                validate();
                            }
                        }
                        Connections {
                            target: pollCreationPage
                            onFocusOption: {
                                if(focusIndex === index) answerTextArea.forceActiveFocus()
                            }
                        }

                        IconButton {
                            icon.source: "image://theme/icon-m-remove"
                            onClicked: {
                                pollCreationPage.options.remove(index)

                                validate();
                            }
                        }
                    }
                }
            }
            ButtonLayout {
                Button {
                    enabled: pollCreationPage.options.count < 10
                    text: qsTr("Add an answer")
                    onClicked: {
                        pollCreationPage.createNewOption();
                        validate();
                    }
                }
            }
            Timer {
                id: focusLastOptionTimer
                interval: 20
                onTriggered: {
                    pollCreationPage.focusOption(pollCreationPage.options.count - 1);
                }
            }

            SectionHeader {
                text: qsTr("Poll Options", "Section header")
            }
            TextSwitch {
                id: anonymousSwitch
                text: qsTr("Anonymous answers")
            }
            TextSwitch {
                id: multipleSwitch
                text: qsTr("Multiple answers allowed")
                onCheckedChanged: {
                    if(checked) {
                        quizSwitch.checked = false
                    }
                }
            }
            TextSwitch {
                id: quizSwitch
                text: qsTr("Quiz Mode")
                onCheckedChanged: {
                    if(checked) {
                        multipleSwitch.checked = false
                    }
                    validate();
                }
                description: qsTr("Quizzes have one correct answer. Participants can't revoke their responses.")
            }
        }
    }

    onAccepted: {
        var optionsArr = [];
        for(var i = 0; i < options.count; i += 1) {
            optionsArr.push(options.get(i).text);
        }

        tdLibWrapper.sendPollMessage(chatId, pollQuestion, optionsArr, anonymous, quiz ? correctOption : -1, multiple, "0");

    }


}
