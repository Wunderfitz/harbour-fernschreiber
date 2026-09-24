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
import Sailfish.Share 1.0
import WerkWolf.Fernschreiber 1.0
import "pages"
import "components"
import "./js/functions.js" as Functions
import "./js/debug.js" as Debug

ApplicationWindow
{
    id: appWindow

    initialPage: Qt.resolvedUrl("pages/OverviewPage.qml")
    cover: Qt.resolvedUrl("pages/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations

    // A share triggered before TDLib is ready to open a chat is held here
    // and replayed once onAuthorizationStateChanged reports AuthorizationReady.
    property var pendingSharedFilePaths: null

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
            if (pendingSharedFilePaths && tdLibWrapper.authorizationState === TelegramAPI.AuthorizationReady) {
                openSharedFiles(pendingSharedFilePaths);
                pendingSharedFilePaths = null;
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

    function openSharedFiles(filePaths) {
        var contentType = classifyContentType(filePaths);
        var headerDescription = contentType === "photo" ? qsTr("Send Image")
            : contentType === "video" ? qsTr("Send Video")
            : qsTr("Send File");
        var neededPermissions = contentType === "photo" ? ["can_send_photos"]
            : contentType === "video" ? ["can_send_videos"]
            : ["can_send_documents"];
        pageStack.push(Qt.resolvedUrl("pages/ChatSelectionPage.qml"), {
            headerDescription: headerDescription,
            payload: {filePaths: filePaths, contentType: contentType, neededPermissions: neededPermissions},
            state: "shareFiles"
        });
    }

    ShareProvider {
        method: "file"
        registerName: true
        capabilities: ["*", "application/*", "audio/*", "font/*", "haptics/*", "image/*", "message/*", "model/*", "multipart/*", "text/*", "video/*"]
        onTriggered: {
            Debug.log("ShareProvider triggered", JSON.stringify(resources));
            appWindow.activate();
            var filePaths = resources.filter(function(resource) { return !!resource.filePath; })
                                      .map(function(resource) { return resource.filePath; });
            if (filePaths.length > 0) {
                // TDLib may still be starting up when the app is share-launched
                // cold; pushing the chat picker before it's ready would race an
                // empty or stale chat list, so the request waits its turn.
                if (tdLibWrapper.authorizationState === TelegramAPI.AuthorizationReady) {
                    openSharedFiles(filePaths);
                } else {
                    pendingSharedFilePaths = filePaths;
                }
            }
        }
    }

    AppNotification {
        id: appNotification
        parent: pageStack.currentPage
    }

    Component.onCompleted: {
        Functions.setGlobals({
            tdLibWrapper: tdLibWrapper,
            appNotification: appNotification
        });
    }
}
