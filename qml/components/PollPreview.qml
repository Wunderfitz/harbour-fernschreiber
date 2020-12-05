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

import "../js/functions.js" as Functions
import "../js/twemoji.js" as Emoji

Item {
    id: pollMessageComponent

    property ListItem messageListItem
    property MessageOverlayFlickable overlayFlickable
    readonly property var rawMessage: messageListItem ? messageListItem.myMessage : overlayFlickable.overlayMessage
    readonly property string chatId: rawMessage.chat_id
    readonly property bool isOwnMessage: messageListItem ? messageListItem.isOwnMessage : overlayFlickable.isOwnMessage
    readonly property string messageId: rawMessage.id
    readonly property bool canEdit: rawMessage.can_be_edited
    readonly property var pollData: rawMessage.content.poll
    property var chosenPollData:({})
    property var chosenIndexes: []
    readonly property bool hasAnswered: {
        return pollData.options.filter(function(option){
            return option.is_chosen
        }).length > 0;
    }
    readonly property bool canAnswer: !hasAnswered && !pollData.is_closed
    readonly property bool isQuiz: pollData.type['@type'] === "pollTypeQuiz"
    property bool highlighted
    width: parent.width
    height: pollColumn.height
    property list<NamedAction> extraContextMenuItems: [
        NamedAction {
            visible: !pollData.is_closed && pollMessageComponent.canEdit
            name: qsTr("Close Poll")
            action: function () { tdLibWrapper.stopPoll(pollMessageComponent.chatId, pollMessageComponent.messageId) }
        },
        NamedAction {
            visible: !pollData.is_closed && !pollMessageComponent.isQuiz && pollMessageComponent.hasAnswered
            name: qsTr("Reset Answer")
            action: function () { pollMessageComponent.resetChosen() }
        }
    ]

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
            textFormat: Text.StyledText
            color: pollMessageComponent.isOwnMessage || pollMessageComponent.highlighted ? Theme.highlightColor : Theme.primaryColor
        }

        Label {
            font.pixelSize: Theme.fontSizeTiny
            width: parent.width
            visible: text !== ""
            text: pollData.is_closed ? qsTr("Final Result:") : (pollData.type.allow_multiple_answers ? qsTr("Multiple Answers are allowed.") : "")
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            color: pollMessageComponent.isOwnMessage || pollMessageComponent.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
        }

        Item {
            visible: !pollMessageComponent.canAnswer
            width: 1
            height: Theme.paddingSmall
        }

        Component {
            id: canAnswerDelegate
            TextSwitch {
                id: optionDelegate
                width: pollMessageComponent.width
                automaticCheck: false
                text: Emoji.emojify(modelData.text, Theme.fontSizeMedium)
                checked: pollMessageComponent.chosenIndexes.indexOf(index) > -1
                highlighted: pollMessageComponent.highlighted || down
                onClicked: {
                    pollMessageComponent.handleChoose(index);
                }
            }
        }

        Component {
            id: resultDelegate
            Row {
                id: optionDelegate
                height: Math.max(Theme.itemSizeExtraSmall, implicitHeight)

                Item {
                    height: parent.height
                    width: Theme.horizontalPageMargin/2
                    anchors.bottom: parent.bottom

                    Rectangle {
                        id: displayOptionChosenMarker
                        anchors.fill: parent
                        color: Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)
                        visible: modelData.is_chosen
                    }

                    OpacityRampEffect {
                        sourceItem: displayOptionChosenMarker
                        direction: OpacityRamp.LeftToRight
                    }
                }

                Item {
                    width: Theme.paddingMedium + (pollMessageComponent.isQuiz ? Theme.iconSizeSmall : 0)
                    height: correctAnswerIcon.height
                    anchors {
                        bottom: parent.bottom
                        bottomMargin: (votePercentageBar.height - height)/2
                    }
                    Icon {
                        id: correctAnswerIcon
                        anchors.bottom: parent.bottom
                        highlighted: pollMessageComponent.isOwnMessage || pollMessageComponent.highlighted
                        readonly property bool isRight: pollMessageComponent.isQuiz && pollData.type.correct_option_id === index
                        source: "image://theme/icon-s-accept"
                        visible: isRight
                    }
                }

                Column {
                    width: list.width - x
                    anchors.bottom: parent.bottom

                    Item {
                        width: parent.width
                        height: Math.max(voteTextLabel.height, votePercentageLabel.y + votePercentageLabel.height)

                        Label {
                            id: voteTextLabel
                            property int lastLineWidth
                            width: parent.width
                            text: Emoji.emojify(modelData.text, Theme.fontSizeMedium)
                            textFormat: Text.StyledText
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            color: pollMessageComponent.isOwnMessage || pollMessageComponent.highlighted ? Theme.highlightColor : Theme.primaryColor
                            onTextChanged: lastLineWidth = 0
                            onLineLaidOut: {
                                if (line.isLast) {
                                    lastLineWidth = line.x + line.implicitWidth
                                }
                            }
                        }

                        Label {
                            id: votePercentageLabel
                            y: (voteTextLabel.lastLineWidth + Theme.paddingLarge) > x ? voteTextLabel.height : Math.max(0, voteTextLabel.height - height)
                            font.pixelSize: Theme.fontSizeTiny
                            text: qsTr("%Ln\%", "% of votes for option", modelData.vote_percentage)
                            horizontalAlignment: Text.AlignRight
                            anchors.right: parent.right
                            color: pollMessageComponent.isOwnMessage || pollMessageComponent.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                        }
                    }

                    Rectangle {
                        id: votePercentageBar
                        height: Theme.paddingSmall
                        width: parent.width
                        color: Theme.rgba(pollMessageComponent.isOwnMessage || pollMessageComponent.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor, 0.3)
                        radius: height/2
                        Rectangle {
                            height: parent.height
                            color: pollMessageComponent.isOwnMessage || pollMessageComponent.highlighted ? Theme.highlightColor : Theme.primaryColor
                            radius: height/2
                            width: parent.width * modelData.vote_percentage * 0.01
                        }
                    }
                }
            }
        }

        ListView {
            id: list
            model: pollData.options
            x: -Theme.horizontalPageMargin/2
            width: parent.width - x
            height: contentHeight
            interactive: false
            delegate: pollMessageComponent.canAnswer ? canAnswerDelegate : resultDelegate
        }

        Item {
            width: 1
            height: Theme.paddingSmall
        }

        Item {
            x: -Theme.horizontalPageMargin/2
            width: parent.width - x
            height: Math.max(voteButtons.height, totalVoterCount.height + Theme.paddingSmall)

            Row {
                id: voteButtons
                spacing: Theme.paddingSmall
                width: parent.width - totalVoterCount.width - parent.spacing
                anchors {
                    left: parent.left
                    bottom: parent.bottom
                }

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

            Label {
                id: totalVoterCount
                font.pixelSize: Theme.fontSizeTiny
                anchors {
                    top: parent.top
                    right: parent.right
                    left: voteButtons.right
                    leftMargin: Theme.paddingMedium
                }
                text: qsTr("%Ln vote(s) total", "number of total votes", pollData.total_voter_count)
                width: contentWidth
                height: contentHeight
                horizontalAlignment: Text.AlignRight
                color: pollMessageComponent.isOwnMessage || pollMessageComponent.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
            }
        }
    }
}
