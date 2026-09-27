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
import "pages"
import "components"
import "./js/functions.js" as Functions

ApplicationWindow
{
    id: appWindow

    initialPage: Qt.resolvedUrl("pages/OverviewPage.qml")
    cover: Qt.resolvedUrl("pages/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations

    // Emitted by every media player that starts playing, so all others pause
    signal mediaPlaybackStarted(var player)

    // A share triggered before TDLib is ready to open a chat is held here
    // and replayed once onAuthorizationStateChanged reports AuthorizationReady.
    property var pendingShare: null

    Connections {
        target: dBusAdaptor
        onPleaseOpenMessage: {
            appWindow.activate();
        }
        onPleaseOpenUrl: {
            appWindow.activate();
        }
    }

    Connections {
        target: tdLibWrapper
        onOpenFileExternally: {
            Qt.openUrlExternally(filePath);
        }
        onTgUrlFound: {
            Functions.handleLink(tgUrl);
        }
        onAuthorizationStateChanged: {
            if (pendingShare && tdLibWrapper.authorizationState === TelegramAPI.AuthorizationReady) {
                openShare(pendingShare);
                pendingShare = null;
            }
        }
    }

    // Only one X-Share Method exists, so the content type isn't known from
    // which one fired - it's read from each file's real mime type instead.
    // A mixed selection falls back to "document", which accepts anything.
    function classifyContentType(filePaths) {
        var types = filePaths.map(function(filePath) {
            var mimeType = fernschreiberUtils.mimeTypeForFile(filePath);
            if (mimeType.indexOf("image/") === 0) {
                return "photo";
            } else if (mimeType.indexOf("video/") === 0) {
                return "video";
            } else {
                return "document";
            }
        });
        return types.every(function(type) { return type === types[0]; }) ? types[0] : "document";
    }

    // Shared text and links go into the message field, next to shared files
    // as their caption.
    function openShare(share) {
        if (share.filePaths.length === 0) {
            pageStack.push(Qt.resolvedUrl("pages/ChatSelectionPage.qml"), {
                payload: {text: share.text, neededPermissions: ["can_send_basic_messages"]},
                state: "fillTextArea"
            });
            return;
        }
        var contentType = classifyContentType(share.filePaths);
        var headerDescription = contentType === "photo" ? qsTr("Send Image")
            : contentType === "video" ? qsTr("Send Video")
            : qsTr("Send File");
        var neededPermissions = contentType === "photo" ? ["can_send_photos"]
            : contentType === "video" ? ["can_send_videos"]
            : ["can_send_documents"];
        pageStack.push(Qt.resolvedUrl("pages/ChatSelectionPage.qml"), {
            headerDescription: headerDescription,
            payload: {filePaths: share.filePaths, contentType: contentType, text: share.text, neededPermissions: neededPermissions},
            state: "shareFiles"
        });
    }

    Connections {
        target: shareReceiver
        onPleaseShare: {
            appWindow.activate();
            // Raw shared data other than text, e.g. a contact, has no file
            // behind it and is written out to a file of its own first.
            var filePaths = [];
            var texts = [];
            resources.forEach(function(resource) {
                if (resource.text) {
                    texts.push(resource.text);
                } else {
                    var filePath = resource.filePath || fernschreiberUtils.writeSharedDataToFile(resource.name, resource.data);
                    if (filePath) {
                        filePaths.push(filePath);
                    }
                }
            });
            var share = {filePaths: filePaths, text: texts.join("\n")};
            if (share.filePaths.length > 0 || share.text) {
                // TDLib may still be starting up when the app is share-launched
                // cold; pushing the chat picker before it's ready would race an
                // empty or stale chat list, so the request waits its turn.
                if (tdLibWrapper.authorizationState === TelegramAPI.AuthorizationReady) {
                    openShare(share);
                } else {
                    pendingShare = share;
                }
            }
        }
    }

    AppNotification {
        id: appNotification
        parent: pageStack.currentPage
    }

    Loader {
        // Only loaded once a contact is about to be stored on the device,
        // reading the address book is nothing to do on every start
        id: deviceContactsLoader
        active: false
        source: Qt.resolvedUrl("components/DeviceContacts.qml")
    }

    Component.onCompleted: {
        Functions.setGlobals({
            tdLibWrapper: tdLibWrapper,
            appNotification: appNotification
        });
    }
}
