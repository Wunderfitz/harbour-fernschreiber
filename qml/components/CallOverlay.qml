/*
    Copyright (C) 2026 Fernschreiber contributors

    This file is part of Fernschreiber and is licensed under the GNU General
    Public License v3.0 (see LICENSE).

    Global overlay for an active Telegram call, driven by the C++ VoipManager
    (context property `voipManager`). Shows the peer, call state, SAS verification
    emojis, remote/local video (for video calls) and the in-call controls.

    The overlay is a direct child of the ApplicationWindow, so it does NOT rotate
    with the page stack. To keep the displayed video upright when the phone is held
    in landscape, the whole call UI is placed in a container that rotates to follow
    the device orientation.
*/
import QtQuick 2.6
import Sailfish.Silica 1.0
import QtMultimedia 5.6
import QtSensors 5.0
import Nemo.DBus 2.0

Rectangle {
    id: callOverlay

    anchors.fill: parent
    z: 10000
    color: Theme.rgba(Theme.highlightDimmerColor, 0.97)
    visible: voiceCallsAvailable && typeof voipManager !== "undefined" && voipManager.active

    property bool videoCall: typeof voipManager !== "undefined" && voipManager.isVideo
    property bool ready: typeof voipManager !== "undefined" && voipManager.callState === "callStateReady"
    // Show the remote video whenever its renderer is actually receiving frames —
    // more reliable than the remoteMediaStateUpdated callback.
    property bool remoteShowing: videoCall && typeof voipManager !== "undefined"
                                 && voipManager.remoteVideo && voipManager.remoteVideo.hasFrame
    // Rotation applied to the whole call UI to match the device orientation.
    property int uiRotation: 0

    // An incoming call rings through ngfd's telephony event rather than through the
    // chat notification settings: a call is not a message, and silencing message
    // tones must not silence the phone ringing. Sound and vibration then follow the
    // system profile, exactly like the built-in dialer. ngfd is on the system bus.
    // Deliberately NOT tied to `visible`: Qt Quick reports effective visibility, so
    // minimising the app to its cover makes this item invisible — and the phone has
    // to ring precisely then. Only the call state may decide.
    readonly property bool ringing: voiceCallsAvailable && typeof voipManager !== "undefined"
                                    && voipManager.active
                                    && !voipManager.isOutgoing
                                    && voipManager.callState === "callStatePending"
    property int _ringtoneEventId: 0

    onRingingChanged: ringing ? startRingtone() : stopRingtone()
    Component.onDestruction: stopRingtone()

    DBusInterface {
        id: callFeedback
        bus: DBus.SystemBus
        service: "com.nokia.NonGraphicFeedback1.Backend"
        path: "/com/nokia/NonGraphicFeedback1"
        iface: "com.nokia.NonGraphicFeedback1"
    }

    function startRingtone() {
        if (callOverlay._ringtoneEventId !== 0) {
            return;
        }
        callFeedback.typedCall("Play",
            [ { "type": "s", "value": "voip_ringtone" }, { "type": "a{sv}", "value": {} } ],
            function(eventId) {
                // The call can be answered or dropped while Play is still in flight;
                // without this the device would keep ringing through the conversation.
                if (callOverlay.ringing) {
                    callOverlay._ringtoneEventId = eventId;
                } else {
                    callFeedback.typedCall("Stop", [ { "type": "u", "value": eventId } ]);
                }
            },
            function() { /* no ngfd: stay silent rather than break the call */ });
    }

    function stopRingtone() {
        if (callOverlay._ringtoneEventId === 0) {
            return;
        }
        callFeedback.typedCall("Stop", [ { "type": "u", "value": callOverlay._ringtoneEventId } ]);
        callOverlay._ringtoneEventId = 0;
    }

    property string peerName: {
        if (!visible || !voipManager.peerUserId) {
            return "";
        }
        var info = tdLibWrapper.getUserInformation("" + voipManager.peerUserId);
        if (!info) {
            return qsTr("Unknown");
        }
        var name = ((info.first_name || "") + " " + (info.last_name || "")).trim();
        return name.length > 0 ? name : qsTr("Unknown");
    }

    // Swallow touches on the background so the page underneath is inert.
    MouseArea { anchors.fill: parent }

    OrientationSensor {
        active: callOverlay.visible
        onReadingChanged: {
            switch (reading.orientation) {
            case OrientationReading.TopUp:    callOverlay.uiRotation = 0; break;
            case OrientationReading.TopDown:  callOverlay.uiRotation = 180; break;
            case OrientationReading.LeftUp:   callOverlay.uiRotation = 270; break;
            case OrientationReading.RightUp:  callOverlay.uiRotation = 90; break;
            // FaceUp/FaceDown: keep the previous orientation (phone lying flat).
            }
        }
    }

    Item {
        id: content
        anchors.centerIn: parent
        width: (callOverlay.uiRotation % 180 === 0) ? parent.width : parent.height
        height: (callOverlay.uiRotation % 180 === 0) ? parent.height : parent.width
        rotation: callOverlay.uiRotation

        // Up to three buttons are visible at once — size them to fit the width.
        property real buttonWidth: (width - 2 * Theme.horizontalPageMargin - 2 * Theme.paddingMedium) / 3

        // Remote video fills the UI. It stays visible for the whole video call so
        // its video surface is assigned to the renderer immediately — otherwise
        // frames arrive but are never presented (surface only exists while visible)
        // and hasFrame never flips. The name/placeholder below is drawn on top
        // until the first remote frame arrives.
        VideoOutput {
            anchors.fill: parent
            fillMode: VideoOutput.PreserveAspectCrop
            source: (typeof voipManager !== "undefined") ? voipManager.remoteVideo : null
            visible: callOverlay.videoCall
        }

        // Peer name + state (shown when there is no remote video to show).
        Column {
            anchors {
                verticalCenter: parent.verticalCenter
                horizontalCenter: parent.horizontalCenter
            }
            width: parent.width - 2 * Theme.horizontalPageMargin
            spacing: Theme.paddingLarge
            visible: !callOverlay.remoteShowing

            Label {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeLarge
                color: Theme.primaryColor
                text: callOverlay.peerName
            }

            Label {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                color: Theme.secondaryHighlightColor
                text: {
                    if (typeof voipManager === "undefined") {
                        return "";
                    }
                    switch (voipManager.callState) {
                    case "callStatePending":
                        return voipManager.isOutgoing ? qsTr("Calling…") : (callOverlay.videoCall ? qsTr("Incoming video call") : qsTr("Incoming call"));
                    case "callStateExchangingKeys":
                        return qsTr("Exchanging encryption keys…");
                    case "callStateReady":
                        return qsTr("Ongoing call");
                    case "callStateHangingUp":
                        return qsTr("Hanging up…");
                    default:
                        return "";
                    }
                }
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: Theme.paddingMedium
                visible: callOverlay.ready
                Repeater {
                    model: visible && typeof voipManager !== "undefined" ? voipManager.emojis : []
                    Label {
                        text: modelData
                        font.pixelSize: Theme.fontSizeExtraLarge
                    }
                }
            }
        }

        // Local camera preview (picture-in-picture) during a video call. Size is
        // tied to the short screen edge so it stays the same in portrait/landscape.
        Rectangle {
            width: Math.min(parent.width, parent.height) * 0.28
            height: width * 4 / 3
            anchors {
                bottom: parent.bottom
                right: parent.right
                rightMargin: Theme.paddingLarge
                bottomMargin: Theme.itemSizeLarge * 1.4
            }
            color: "black"
            radius: Theme.paddingSmall
            clip: true
            visible: callOverlay.videoCall && callOverlay.ready
            VideoOutput {
                anchors.fill: parent
                fillMode: VideoOutput.PreserveAspectCrop
                source: (typeof voipManager !== "undefined") ? voipManager.localVideo : null
            }
        }

        Row {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: Theme.paddingLarge * 2
            }
            spacing: Theme.paddingMedium

            Button {
                width: content.buttonWidth
                text: qsTr("Accept")
                visible: typeof voipManager !== "undefined"
                         && !voipManager.isOutgoing
                         && voipManager.callState === "callStatePending"
                onClicked: voipManager.acceptCall()
            }

            Button {
                id: muteButton
                width: content.buttonWidth
                property bool muted: false
                text: muted ? qsTr("Unmute") : qsTr("Mute")
                visible: callOverlay.ready
                onClicked: {
                    muted = !muted;
                    voipManager.setMicrophoneMuted(muted);
                }
            }

            Button {
                width: content.buttonWidth
                text: qsTr("Flip")
                visible: callOverlay.videoCall && callOverlay.ready
                onClicked: voipManager.switchCamera()
            }

            Button {
                width: content.buttonWidth
                text: (typeof voipManager !== "undefined"
                       && !voipManager.isOutgoing
                       && voipManager.callState === "callStatePending")
                      ? qsTr("Decline") : qsTr("Hang up")
                onClicked: voipManager.hangUp()
            }
        }
    }

    onVisibleChanged: if (visible) { muteButton.muted = false; }
}
