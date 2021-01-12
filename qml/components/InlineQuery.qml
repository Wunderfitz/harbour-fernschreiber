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
import "../js/debug.js" as Debug
import "../js/twemoji.js" as Emoji
import "../js/functions.js" as Functions

Loader {
    id: inlineQueryLoader
    active: userName.length > 1
    asynchronous: true
    anchors {
        left: parent.left
        right: parent.right
        top: parent.top
        bottom: active ? parent.bottom : parent.top
    }
    property bool hasOverlay: active && userNameIsValid && status === Loader.Ready && item.overlay && item.overlay.status === Loader.Ready
    property bool hasButton: active && userNameIsValid && status === Loader.Ready && item.button && item.button.status === Loader.Ready

    property int buttonPadding: hasButton ? item.button.height + Theme.paddingSmall : 0
    Behavior on buttonPadding { NumberAnimation { duration: 200} }

    property string chatId
    property string userName
    property bool userNameIsValid: userName !== "" && inlineBotInformation && userName === inlineBotInformation.username
    property string query
    property int currentOffset: 0
    property string responseExtra: chatId+"|"+userName+"|"+query+"|"+currentOffset

    property bool queued: false
    property TextArea textField
    property bool isLoading
    property var inlineBotInformation: null
    onIsLoadingChanged: {
        requestTimeout.start();
    }

    onStatusChanged: {
        inlineBotInformation = null;
        if(status === Loader.Ready && userName !== "") {
            isLoading = true; inlineQueryLoader.chatId
            tdLibWrapper.searchPublicChat(userName, false);
        }
    }

    onUserNameChanged: {
        inlineBotInformation = null;

        if(status === Loader.Ready && userName !== "") {
            isLoading = true;
            tdLibWrapper.searchPublicChat(userName, false);
        }
    }

    onQueryChanged: {
        if(userName.length > 0) {
            isLoading = true;
            requestTimer.start();
        }
    }

    function handleQuery(name, query, offset) {
        if(!name) {
            inlineQueryLoader.userName = "";
            inlineQueryLoader.query = "";
            return;
        }
        if(inlineQueryLoader.userName !== name) {
            inlineQueryLoader.userName = name
        }
        if(inlineQueryLoader.query !== query) {
            inlineQueryLoader.query = query
        }
        inlineQueryLoader.currentOffset = offset || 0
    }

    function request() {
        if(!inlineBotInformation || !userNameIsValid) {
            queued = true;
        } else {
            queued = false;
            var location = null;
            if(inlineBotInformation.type.need_location && fernschreiberUtils.supportsGeoLocation()) {
                fernschreiberUtils.startGeoLocationUpdates();
                if(!attachmentPreviewRow.locationData.latitude) {
                    queued = true;
                    return;
                }
            }
            tdLibWrapper.getInlineQueryResults(inlineBotInformation.id, chatId, location, query, inlineQueryLoader.currentOffset, inlineQueryLoader.responseExtra);
            isLoading = true;
        }
    }

    Timer {
        id: requestTimeout
        interval: 5000
        onTriggered: {
            inlineQueryLoader.isLoading = false;
        }
    }

    Timer {
        id: requestTimer
        interval: 1000
        onTriggered: {
            request();
        }
    }

    Connections {
        target: fernschreiberUtils
        onNewPositionInformation: {
            attachmentPreviewRow.locationData = positionInformation;
            if (inlineQueryLoader.queued) {
                inlineQueryLoader.queued = false;
                inlineQueryLoader.request()
            }
        }
    }

    Connections {
        target: textField
        onTextChanged: {
            if(textField.text.charAt(0) === '@') {
                var queryMatch = textField.text.match(/^@([a-zA-Z0-9_]+)\s(.*)/);
                if(queryMatch) {
                    inlineQueryLoader.handleQuery(queryMatch[1], queryMatch[2]);
                } else {
                    inlineQueryLoader.handleQuery();
                }
            } else {
                inlineQueryLoader.handleQuery();
            }
        }
    }

    sourceComponent: Component {
        Item {
            id: inlineQueryComponent
            anchors.fill: parent
            property alias overlay: resultsOverlay
            property alias button: switchToPmLoader
            property string nextOffset
            property string inlineQueryId
            property string switchPmText
            property string switchPmParameter
            property ListModel resultModel: ListModel {
                dynamicRoles: true
            }
            property string inlineQueryPlaceholder: inlineBotInformation ? inlineBotInformation.type.inline_query_placeholder : ""
            property bool showInlineQueryPlaceholder: !!inlineQueryPlaceholder && query === ""
            property string useDelegateSize: "default"
            property var dimensions: ({
                                          "default": [[Screen.width, Screen.height / 2], [Theme.itemSizeLarge, Theme.itemSizeLarge]], // whole line (portrait half)
                                          "inlineQueryResultAnimation":  [[Screen.width / 3, Screen.height / 6], [Screen.width / 3, Screen.height / 6]],
                                          "inlineQueryResultVideo":  [[Screen.width / 2, Screen.height / 4], [Theme.itemSizeLarge, Theme.itemSizeLarge]],
                                          "inlineQueryResultSticker":  [[Screen.width / 3, Screen.height / 6], [Screen.width / 3, Screen.height / 6]],
                                          "inlineQueryResultPhoto":  [[Screen.width/2, Screen.height / 3], [Theme.itemSizeExtraLarge, Theme.itemSizeExtraLarge]],
                                      })
            property int delegateWidth: chatPage.isPortrait ? dimensions[useDelegateSize][0][0] : dimensions[useDelegateSize][0][1]
            property int delegateHeight: chatPage.isPortrait ? dimensions[useDelegateSize][1][0] : dimensions[useDelegateSize][1][1]

            function setDelegateSizes() {
                var sizeKey = "default";
                var modelCount = resultModel.count;
                if(modelCount > 0) {
                    var firstType = resultModel.get(0)["@type"];
                    if(firstType && dimensions[firstType]) {
                        var startIndex = inlineQueryLoader.currentOffset === 0 ? 1 : inlineQueryLoader.currentOffset;
                        var same = true;
                        for(var i = startIndex; i < modelCount; i += 1) {
                            if(resultModel.get(i)["@type"] !== firstType) {
                                same = false;
                                continue;
                            }
                        }
                        if(same) {
                            sizeKey = firstType;
                        }
                    }
                }
                useDelegateSize = sizeKey;
            }

            function loadMore() {
                if(nextOffset && inlineQueryLoader.userNameIsValid) {
                    inlineQueryLoader.currentOffset = nextOffset;
                    inlineQueryLoader.request();
                }
            }

            Connections {
                target: tdLibWrapper

                onChatReceived: {
                    if(chat["@extra"] === "searchPublicChat:"+inlineQueryLoader.userName) {
                        requestTimeout.stop();
                        inlineQueryLoader.isLoading = false;
                        var inlineBotInformation = tdLibWrapper.getUserInformation(chat.type.user_id);
                        if(inlineBotInformation && inlineBotInformation.type["@type"] === "userTypeBot" && inlineBotInformation.type.is_inline) {
                            inlineQueryLoader.inlineBotInformation = inlineBotInformation;
                            requestTimer.start();
                        }
                    }
                }
                onInlineQueryResults: {
                    if(extra === inlineQueryLoader.responseExtra) {
                        requestTimeout.stop();
                        inlineQueryLoader.isLoading = false;
                        inlineQueryComponent.inlineQueryId = inlineQueryId
                        inlineQueryComponent.nextOffset = nextOffset
                        inlineQueryComponent.switchPmText = switchPmText
                        inlineQueryComponent.switchPmParameter = switchPmParameter

                        if(inlineQueryLoader.currentOffset === 0) {
                            inlineQueryComponent.resultModel.clear()
                        }
                        for(var i = 0; i < results.length; i++) {
                            inlineQueryComponent.resultModel.append(results[i]);
                        }

                        if(inlineQueryLoader.currentOffset === 0 || inlineQueryLoader.useDelegateSize !== "default") {
                            inlineQueryComponent.setDelegateSizes()
                        }
                    }
                }
            }
            // switch to pm Button
            Loader {
                id: switchToPmLoader
                asynchronous: true
                active: inlineQueryComponent.switchPmText.length > 0
                opacity: status === Loader.Ready ? 1.0 : 0.0
                Behavior on opacity { FadeAnimation {} }
                height: Theme.itemSizeSmall
                anchors {
                    top: parent.bottom
                    topMargin: Theme.paddingSmall
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                }
                sourceComponent: Component {
                    MouseArea {
                        id: customButton
                        onClicked: {
                            tdLibWrapper.createPrivateChat(inlineQueryLoader.inlineBotInformation.id, "openAndSendStartToBot:"+(inlineQueryComponent.switchPmParameter.length > 0 ? " "+inlineQueryComponent.switchPmParameter:""));
                        }
                        Rectangle {
                            anchors.fill: parent
                            radius: Theme.paddingSmall
                            color: parent.pressed ? Theme.highlightBackgroundColor : Theme.rgba(Theme.DarkOnLight ? Qt.lighter(Theme.primaryColor) : Qt.darker(Theme.primaryColor), Theme.opacityFaint)
                            Label {
                                anchors {
                                    fill: parent
                                    leftMargin: Theme.paddingLarge
                                    rightMargin: Theme.paddingLarge
                                }
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter

                                fontSizeMode: Text.Fit;
                                minimumPixelSize: Theme.fontSizeTiny;
                                font.pixelSize: Theme.fontSizeSmall

                                color: customButton.pressed ? Theme.highlightColor : Theme.primaryColor
                                text: Emoji.emojify(inlineQueryComponent.switchPmText, font.pixelSize)// + "we are gonna make this a bit longer"
                            }
                        }
                    }
                }
            }

            // results grid overlay
            Loader {
                id: resultsOverlay
                asynchronous: true
                active: inlineQueryComponent.resultModel.count > 0
                anchors.fill: parent
                opacity: !!item ? 1.0 : 0.0
                Behavior on opacity { FadeAnimation {} }
                property var supportedResultTypes: [
                    "inlineQueryResultAnimation",
                    "inlineQueryResultArticle",
                    "inlineQueryResultAudio",
                    "inlineQueryResultContact",
                    "inlineQueryResultDocument",
                    "inlineQueryResultGame",
                    "inlineQueryResultLocation",
                    "inlineQueryResultPhoto",
                    "inlineQueryResultSticker",
                    "inlineQueryResultVenue",
                    "inlineQueryResultVideo",
                    "inlineQueryResultVoiceNote",
                ]
                sourceComponent: Component {
                    Item {
                        Rectangle {
                            id: messageContentBackground
                            color: Theme.overlayBackgroundColor
                            opacity: 0.7
                            anchors.fill: parent
                        }
                        Timer {
                            id: autoLoadMoreTimer
                            interval: 400
                            onTriggered: {
                                if (inlineQueryComponent.nextOffset && resultView.height > resultView.contentHeight - Theme.itemSizeHuge) {
                                    inlineQueryComponent.loadMore();
                                }
                            }
                        }
                        SilicaGridView {
                            id: resultView
                            anchors.fill: parent
                            cellWidth: inlineQueryComponent.delegateWidth
                            cellHeight: inlineQueryComponent.delegateHeight

                            signal requestPlayback(url playbackSource)
                            clip: true
                            model: inlineQueryComponent.resultModel
                            delegate: Loader {
                                id: queryResultDelegate
                                height: resultView.cellHeight
                                width: resultView.cellWidth
                                source: "inlineQueryResults/" + (resultsOverlay.supportedResultTypes.indexOf(model["@type"]) > -1 ? (model["@type"].charAt(0).toUpperCase() + model["@type"].substring(1)) : "InlineQueryResultDefaultBase") +".qml"
                            }
                            footer: Component {
                                Item {
                                    width: resultView.width
                                    visible: height > 0
                                    height: inlineQueryComponent.nextOffset ? Theme.itemSizeLarge : 0
                                    Behavior on height { NumberAnimation { duration: 500 } }
                                }
                            }

                            onContentYChanged: {
                                if(!inlineQueryLoader.isLoading && inlineQueryComponent.nextOffset && contentHeight - contentY - height < Theme.itemSizeHuge) {
                                    inlineQueryComponent.loadMore();
                                }
                            }

                            ScrollDecorator { flickable: resultView }
                        }
                    }
                }
            }


            // textarea placeholder
            Loader {
                asynchronous: true
                active: inlineQueryComponent.showInlineQueryPlaceholder
                sourceComponent: Component {
                    Label {
                        text: Emoji.emojify(inlineQueryComponent.inlineQueryPlaceholder, font.pixelSize);
                        parent: textField
                        anchors.fill: parent
                        anchors.leftMargin: textMetrics.boundingRect.width + Theme.paddingSmall
                        font: textField.font
                        color: Theme.secondaryColor

                        truncationMode: TruncationMode.Fade
                        TextMetrics {
                            id: textMetrics
                            font: textField.font
                            text: textField.text
                        }
                    }
                }
            }


        }
    }



}
