/*
    Copyright (C) 2020-21 Sebastian J. Wolf and other contributors

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
import "../js/twemoji.js" as Emoji
import "../js/debug.js" as Debug

Item {
    id: voiceNoteOverlayItem
    anchors.fill: parent

    property int recordingState: fernschreiberUtils.getVoiceNoteRecordingState();
    property int recordingDuration: 0;
    property bool recordingDone: false;

    function handleRecordingState() {
        switch (recordingState) {
        case FernschreiberUtilities.Unavailable:
            recordingStateLabel.text = qsTr("Unavailable");
            break;
        case FernschreiberUtilities.Ready:
            recordingStateLabel.text = qsTr("Ready");
            break;
        case FernschreiberUtilities.Starting:
            recordingStateLabel.text = qsTr("Starting");
            break;
        case FernschreiberUtilities.Recording:
            recordingStateLabel.text = qsTr("Recording");
            break;
        case FernschreiberUtilities.Stopping:
            recordingStateLabel.text = qsTr("Stopping");
            break;
        }
    }

    function getTwoDigitString(numberToBeConverted) {
        var numberString = "00";
        if (numberToBeConverted > 0 && numberToBeConverted < 10) {
            numberString = "0" + String(numberToBeConverted);
        }
        if (numberToBeConverted >= 10) {
            numberString = String(numberToBeConverted);
        }
        return numberString;
    }

    function handleRecordingDuration() {
        var minutes = Math.floor(recordingDuration / 60);
        var seconds = recordingDuration % 60;
        recordingDurationLabel.text = getTwoDigitString(minutes) + ":" + getTwoDigitString(seconds);
    }

    Component.onCompleted: {
        handleRecordingState();
        handleRecordingDuration();
    }

    Connections {
        target: fernschreiberUtils
        onVoiceNoteDurationChanged: {
            Debug.log("New duration received: " + duration);
            recordingDuration = Math.round(duration / 1000);
            handleRecordingDuration();
        }
        onVoiceNoteRecordingStateChanged: {
            Debug.log("New state received: " + state);
            recordingState = state;
            handleRecordingState();
        }
    }

    Rectangle {
        id: stickerPickerOverlayBackground
        anchors.fill: parent

        color: Theme.overlayBackgroundColor
        opacity: Theme.opacityHigh
    }

    Flickable {
        id: voiceNoteFlickable
        anchors.fill: parent
        anchors.margins: Theme.paddingMedium

        Behavior on opacity { NumberAnimation {} }

        contentHeight: voiceNoteColumn.height
        clip: true

        Column {
            id: voiceNoteColumn
            spacing: Theme.paddingMedium
            width: voiceNoteFlickable.width

            InfoLabel {
                text: qsTr("Record a Voice Note")
            }

            Label {
                wrapMode: Text.Wrap
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Press the button to start recording")
                font.pixelSize: Theme.fontSizeMedium
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Item {
                width: Theme.iconSizeExtraLarge
                height: Theme.iconSizeExtraLarge
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                Rectangle {
                    color: Theme.primaryColor
                    opacity: Theme.opacityOverlay
                    width: Theme.iconSizeExtraLarge
                    height: Theme.iconSizeExtraLarge
                    anchors.centerIn: parent
                    radius: width / 2
                }

                Rectangle {
                    id: recordButton
                    color: "red"
                    width: Theme.iconSizeExtraLarge * 0.6
                    height: Theme.iconSizeExtraLarge * 0.6
                    anchors.centerIn: parent
                    radius: width / 2
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            recordButton.visible = false;
                            fernschreiberUtils.startRecordingVoiceNote();
                        }
                    }
                }

                Rectangle {
                    id: stopButton
                    visible: !recordButton.visible
                    color: Theme.overlayBackgroundColor
                    width: Theme.iconSizeExtraLarge * 0.4
                    height: Theme.iconSizeExtraLarge * 0.4
                    anchors.centerIn: parent
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            recordButton.visible = true;
                            fernschreiberUtils.stopRecordingVoiceNote();
                            recordingDone = true;
                        }
                    }
                }
            }

            Label {
                id: recordingStateLabel
                wrapMode: Text.Wrap
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Theme.fontSizeMedium
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Label {
                id: recordingDurationLabel
                wrapMode: Text.Wrap
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Theme.fontSizeMedium
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Button {
                visible: recordingDone
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                text: qsTr("Use recording")
                onClicked: {
                    attachmentOptionsFlickable.isNeeded = false;
                    attachmentPreviewRow.isVoiceNote = true;
                    attachmentPreviewRow.attachmentDescription = qsTr("Voice Note (%1)").arg(recordingDurationLabel.text);
                    attachmentPreviewRow.visible = true;
                    controlSendButton();
                    voiceNoteOverlayLoader.active = false;
                }
            }

        }
    }

}

