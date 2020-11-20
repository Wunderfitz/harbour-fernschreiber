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
import "../js/functions.js" as Functions
import "../js/twemoji.js" as Emoji
import "../js/debug.js" as Debug

Page {
    id: pollResultsPage
    allowedOrientations: Orientation.All
    property string chatId;
    property var message: ({});

    property string messageId: message.id;

    property var pollData: message.content.poll

    property var userInformation:  tdLibWrapper.getUserInformation(message.sender_user_id)

    property bool isQuiz: pollData.type['@type'] === "pollTypeQuiz"

    property bool hasAnswered: {
        return pollData.options.filter(function(option){
            return option.is_chosen
        }).length > 0;
    }

    property bool canAnswer: !hasAnswered && !pollData.is_closed
    onCanAnswerChanged: {
        if(canAnswer) { // vote removed from another client?
            pageStack.pop()
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: pageHeader.height + contentColumn.height

        PageHeader {
            id: pageHeader
            title: pollResultsPage.isQuiz ? qsTr("Quiz Results") : qsTr("Poll Results")
            description: qsTr("%Ln vote(s) total", "number of total votes", pollData.total_voter_count)
            leftMargin: headerPictureThumbnail.width + Theme.paddingLarge + Theme.horizontalPageMargin
            ProfileThumbnail {
                id: headerPictureThumbnail
                photoData: (typeof pollResultsPage.userInformation.profile_photo !== "undefined") ? pollResultsPage.userInformation.profile_photo.small : ""
                replacementStringHint: Emoji.emojify(Functions.getUserName(pollResultsPage.userInformation), font.pixelSize)
                width: visible ? Theme.itemSizeSmall : 0
                height: visible ? Theme.itemSizeSmall : 0
                anchors {
                    verticalCenter: pageHeader.verticalCenter
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                }
            }
        }
        Column {
            id: contentColumn
            anchors {
                left: parent.left
                leftMargin: Theme.horizontalPageMargin
                right: parent.right
                rightMargin: Theme.horizontalPageMargin
                top: pageHeader.bottom
            }
            SectionHeader {
                x: 0
                text: qsTr("Question", "section header")
            }
//            Label {
//                width: parent.width
//                font.pixelSize: Theme.fontSizeTiny
//                wrapMode: Text.Wrap
//                color: Theme.secondaryHighlightColor
//                text: JSON.stringify(pollData, null, 2)
//            }
//            Label {
//                width: parent.width
//                font.pixelSize: Theme.fontSizeTiny
//                wrapMode: Text.Wrap
//                color: Theme.secondaryHighlightColor
//                text: JSON.stringify(userInformation, null, 2)
//            }
            Label {
                width: parent.width
                wrapMode: Text.Wrap
                color: Theme.secondaryHighlightColor
                text: Emoji.emojify(pollData.question, font.pixelSize)
            }

            Column {
                id: resultsColumn
                width: parent.width
                topPadding: Theme.paddingLarge
                bottomPadding: Theme.paddingLarge

                SectionHeader {
                    x: 0
                    text: qsTr("Results", "section header")
                }
                Repeater {
                    model: pollData.options
                    delegate: Item {
                        id: optionDelegate
                        width: resultsColumn.width
                        height: displayOptionLabel.height + displayOptionStatistics.height + displayOptionUsers.height + Theme.paddingLarge
                        property ListModel users: ListModel {}
                        property string usersResponseIdentifierString: "pollResults."+pollResultsPage.chatId+"."+pollResultsPage.messageId+"."+index
                        function loadUsers() {
                            if(users.count < modelData.voter_count) {
                                tdLibWrapper.getPollVoters(pollResultsPage.chatId, pollResultsPage.messageId, index, 50, users.length, usersResponseIdentifierString)
                            }
                        }
                        Component.onCompleted: {
//                            loadUsers()
                            loadUsersTimer.start()
                        }
                        Timer {
                            id: loadUsersTimer
                            interval: index * 80
                            onTriggered: {
                                optionDelegate.loadUsers();
                            }
                        }

                        Connections {
                            target: tdLibWrapper
                            onUsersReceived: {
                                if(extra === optionDelegate.usersResponseIdentifierString) {
                                    for(var i = 0; i < userIds.length; i += 1) {
                                        optionDelegate.users.append({id: userIds[i], user:tdLibWrapper.getUserInformation(userIds[i])});
                                    }
                                    loadUsersTimer.start();
                                }
                            }
                        }
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
                            width: pollResultsPage.isQuiz ?Theme.iconSizeSmall + Theme.paddingMedium : Theme.paddingMedium
                            height: displayOptionLabel.height + displayOptionStatistics.height
                            anchors {
                                left: parent.left
//                                verticalCenter: parent.verticalCenter
                            }

                            Icon {
                                highlighted: true
                                property bool isRight: pollResultsPage.isQuiz && pollData.type.correct_option_id === index
                                source: "image://theme/icon-s-accept"
                                visible: isRight
                                anchors.verticalCenter: parent.verticalCenter
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
                            color: Theme.highlightColor
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
                                id: optionVoterCount
                                font.pixelSize: Theme.fontSizeTiny
                                text: modelData.is_chosen ? qsTr("%Ln vote(s) including yours", "number of votes for option", modelData.voter_count) : qsTr("%Ln vote(s)", "number of votes for option", modelData.voter_count)
                                anchors {
                                    left: parent.left
                                    right: parent.horizontalCenter
                                    rightMargin: Theme.paddingSmall
                                }
                                color: Theme.secondaryHighlightColor
                            }
                            Label {
                                id: optionVoterPercentage
                                font.pixelSize: Theme.fontSizeTiny
                                text: qsTr("%Ln\%", "% of votes for option", modelData.vote_percentage)
                                horizontalAlignment: Text.AlignRight
                                anchors {
                                    right: parent.right
                                    left: parent.horizontalCenter
                                    leftMargin: Theme.paddingSmall
                                }
                                color: Theme.secondaryHighlightColor
                            }
                            Rectangle {
                                id: optionVoterPercentageBar
                                height: Theme.paddingSmall
                                width: parent.width

                                color: Theme.rgba(Theme.secondaryHighlightColor, 0.3)
                                radius: height/2
                                anchors {
                                    left: parent.left
                                    bottom: parent.bottom
                                }

                                Rectangle {
                                    height: parent.height
                                    color: Theme.highlightColor
                                    radius: height/2
                                    width: parent.width * modelData.vote_percentage * 0.01
                                }
                            }
                        }


                        // users voted for this:
                        Flow {
                            id: displayOptionUsers
                            anchors.top: displayOptionStatistics.bottom
                            width: parent.width
                            visible: optionDelegate.users.count > 0
                            topPadding: Theme.paddingLarge
                            spacing: Theme.paddingMedium
                            leftPadding: iconsColumn.width
                            property int itemHeight: Theme.itemSizeExtraSmall / 2
                            Item {
                                height: displayOptionUsers.itemHeight
                                width: chosenByUserText.width
                                Label {
                                    id: chosenByUserText
                                    font.pixelSize: Theme.fontSizeTiny
                                    text: qsTr("Chosen by:", "This answer has been chosen by the following users")
                                    width: contentWidth
                                    anchors.centerIn: parent
                                    color: Theme.secondaryHighlightColor
                                }
                            }
                            Repeater {
                                model: optionDelegate.users
                                delegate:
                                    Item {
                                    id: chosenByUserItem
                                    width: chosenByUserPictureThumbnail.width + chosenByUserLabel.width + Theme.paddingSmall
                                    height: displayOptionUsers.itemHeight

                                    ProfileThumbnail {
                                        id: chosenByUserPictureThumbnail
                                        photoData: (typeof model.user.profile_photo !== "undefined") ? model.user.profile_photo.small : ""
                                        replacementStringHint: chosenByUserLabel.text
                                        width: visible ? parent.height : 0
                                        height: width
                                        anchors {
                                            left: parent.left
                                            verticalCenter: parent.verticalCenter
                                        }
                                    }

                                    Label {
                                        id: chosenByUserLabel
                                        font.pixelSize: Theme.fontSizeSmall
                                        text: Emoji.emojify(Functions.getUserName(model.user), font.pixelSize)
                                        width: contentWidth
                                        height: contentHeight
                                        color: Theme.highlightColor
                                        anchors {
                                            right: parent.right
                                            verticalCenter: parent.verticalCenter
                                        }
                                    }
                                }


                            }
                        }

                    }
                }

            }

        }
    }

    Connections {
        target: tdLibWrapper
        onMessageContentUpdated: {
            if(chatId === pollResultsPage.chatId && messageId === pollResultsPage.messageId) {
                pollResultsPage.pollData = newContent.poll;
            }
        }
    }

}
