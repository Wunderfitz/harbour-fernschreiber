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

import QtQuick 2.0
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0

import "../js/functions.js" as Functions
import "../js/twemoji.js" as Emoji

Item {
    id: pollMessageComponent

    property string chatId
    property var message:({})
    property bool isOwnMessage

    property string messageId: message.id
    property bool canEdit: message.can_be_edited
    property var pollData: message.content.poll
    property var chosenPollData:({})
    property var chosenIndexes: []
    property bool hasAnswered: {
        return pollData.options.filter(function(option){
            return option.is_chosen
        }).length > 0;
    }
    property bool canAnswer: !hasAnswered && !pollData.is_closed
    property bool isQuiz: pollData.type['@type'] === "pollTypeQuiz"
    property Item messageItem
    height: pollColumn.height
    opacity: 0
    Behavior on opacity { FadeAnimation {} }
    function handleChoose(index) {
        if(!pollData.type.allow_multiple_answers) {
            chosenIndexes = [index];
            sendResponse();
            return;
        }
        var indexes = chosenIndexes;
        var found = indexes.indexOf(index);
        if(found > -1) { // uncheck
            indexes.splice(found, 1);
        } else {
            indexes.push(index)
        }
        chosenIndexes = indexes;
    }
    function resetChosen() {
        chosenIndexes = [];
        sendResponse();
    }
    function sendResponse() {
        tdLibWrapper.setPollAnswer(chatId, messageId, chosenIndexes);
    }

    Column {
        id: pollColumn
        width: parent.width
        spacing: Theme.paddingSmall
        Label {
            font.pixelSize: Theme.fontSizeSmall
            width: parent.width
            visible: text !== ""
            text: Emoji.emojify(pollData.question, Theme.fontSizeSmall)
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            color: pollMessageComponent.isOwnMessage ? Theme.highlightColor : Theme.primaryColor
        }

        Label {
            font.pixelSize: Theme.fontSizeTiny
            width: parent.width
            visible: text !== ""
            text: pollData.is_closed ? qsTr("Final Result:") : (pollData.type.allow_multiple_answers ? qsTr("Multiple Answers are allowed.") : "")
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            color: pollMessageComponent.isOwnMessage ? Theme.secondaryHighlightColor : Theme.secondaryColor
        }

        Item {
            visible: !pollMessageComponent.canAnswer
            width: parent.width
            height: Theme.paddingSmall
        }

        Component {
            id: canAnswerDelegate
            TextSwitch {
                id: optionDelegate
                // TextSwitch changes the html base path:
                property url emojiBase: "../js/emoji/"
                width: pollMessageComponent.width
                automaticCheck: false
                text: Emoji.emojify(modelData.text, Theme.fontSizeMedium, emojiBase)
                checked: pollMessageComponent.chosenIndexes.indexOf(index) > -1
                onClicked: {
                    pollMessageComponent.handleChoose(index);
                }
            }
        }
        Component {
            id: resultDelegate
            Item {
                id: optionDelegate
                width: pollMessageComponent.width
                height: displayOptionLabel.height + displayOptionStatistics.height

                Rectangle {
                    id: displayOptionChosenMarker
                    height: parent.height
                    width: Theme.horizontalPageMargin/2
                    color: Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)
                    visible: modelData.is_chosen
                    x: -width
                }
                OpacityRampEffect {
                    sourceItem: displayOptionChosenMarker
                    direction: OpacityRamp.LeftToRight
                }
                Column {
                    id: iconsColumn
                    width: pollMessageComponent.isQuiz ?Theme.iconSizeSmall + Theme.paddingMedium : Theme.paddingMedium

                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                    }

                    Icon {
                        highlighted: pollMessageComponent.isOwnMessage
                        property bool isRight: pollMessageComponent.isQuiz && pollData.type.correct_option_id === index
                        source: "image://theme/icon-s-accept"
                        visible: isRight
                    }
                }

                Label {
                    id: displayOptionLabel
                    text: Emoji.emojify(modelData.text, Theme.fontSizeMedium)

                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    anchors {
                        left: iconsColumn.right
                        top: parent.top
                        right: parent.right
                    }
                    color: pollMessageComponent.isOwnMessage ? Theme.highlightColor : Theme.primaryColor
                }
                Item {
                    id: displayOptionStatistics
                    height: optionVoterPercentage.height + optionVoterPercentageBar.height
                    anchors {
                        top: displayOptionLabel.bottom
                        left: iconsColumn.right
                        right: parent.right
                    }

                    Label {
                        id: optionVoterPercentage
                        font.pixelSize: Theme.fontSizeTiny
                        text: qsTr("%L1\%", "% of votes for option").arg(modelData.vote_percentage)
                        horizontalAlignment: Text.AlignRight
                        anchors {
                            right: parent.right
                            left: parent.horizontalCenter
                            leftMargin: Theme.paddingSmall
                        }
                        color: pollMessageComponent.isOwnMessage ? Theme.secondaryHighlightColor : Theme.secondaryColor
                    }
                    Rectangle {
                        id: optionVoterPercentageBar
                        height: Theme.paddingSmall
                        width: parent.width

                        color: Theme.rgba(pollMessageComponent.isOwnMessage ? Theme.secondaryHighlightColor : Theme.secondaryColor, 0.3)
                        radius: height/2
                        anchors {
                            left: parent.left
                            bottom: parent.bottom
                        }

                        Rectangle {
                            height: parent.height
                            color: pollMessageComponent.isOwnMessage ? Theme.highlightColor : Theme.primaryColor
                            radius: height/2
                            width: parent.width * modelData.vote_percentage * 0.01
                        }
                    }
                }
            }
        }

        Repeater {
            model: pollData.options
            delegate: pollMessageComponent.canAnswer ? canAnswerDelegate : resultDelegate
        }

        Row {
            layoutDirection: Qt.RightToLeft
            width: parent.width
            spacing: Theme.paddingMedium
            Behavior on height { NumberAnimation {}}


            Label {
                id: totalVoterCount
                font.pixelSize: Theme.fontSizeTiny
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("%L1 vote(s) total", "number of total votes", pollData.total_voter_count).arg(pollData.total_voter_count)
                width: contentWidth
                height: contentHeight
                horizontalAlignment: Text.AlignRight
                color: pollMessageComponent.isOwnMessage ? Theme.secondaryHighlightColor : Theme.secondaryColor
            }

            Row {
                spacing: Theme.paddingSmall
                width: parent.width - totalVoterCount.width - parent.spacing
                IconButton {
                    visible: !pollData.is_closed && pollMessageComponent.chosenIndexes.length > 0 && pollData.type.allow_multiple_answers  && !pollMessageComponent.hasAnswered
                    opacity: visible ? 1.0 : 0.0
                    Behavior on opacity { FadeAnimation {}}
                    icon.source: "image://theme/icon-m-send"
                    onClicked: {
                        pollMessageComponent.sendResponse()
                    }
                }


                IconButton {
                    visible: !pollMessageComponent.canAnswer && !pollData.is_anonymous && pollData.total_voter_count > 0
                    icon.source: "image://theme/icon-m-media-artists"
                    onClicked: {
                        pageStack.push(Qt.resolvedUrl("../pages/PollResultsPage.qml"), { chatId:chatId, message:pollMessageComponent.message});
                    }
                    Icon {
                        opacity: 0.8
                        source: "image://theme/icon-s-maybe"
                        anchors {
                            right: parent.right
                            top: parent.top
                        }
                    }
                }
            }
        }

    }
    Component {
        id: closePollMenuItemComponent
        MenuItem {
            visible: !pollData.is_closed && pollMessageComponent.canEdit
            text: qsTr("Close Poll")
            onClicked: {
                tdLibWrapper.stopPoll(pollMessageComponent.chatId, pollMessageComponent.messageId);
            }
        }
    }
    Component {
        id: resetAnswerMenuItemComponent
        MenuItem {
            visible: !pollData.is_closed && !pollMessageComponent.isQuiz && pollMessageComponent.hasAnswered
            text: qsTr("Reset Answer")
            onClicked: {
                pollMessageComponent.resetChosen()
            }
        }
    }

    Component.onCompleted: {
        opacity = 1;
        if(messageItem && messageItem.menu ) { // workaround to add menu entries
            closePollMenuItemComponent.createObject(messageItem.menu._contentColumn);
            resetAnswerMenuItemComponent.createObject(messageItem.menu._contentColumn);
        }
    }
}
