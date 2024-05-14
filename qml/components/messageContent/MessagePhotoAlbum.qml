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
import "../"

MessageContentBase {
    id: messageContent
    property string chatId
    readonly property int heightUnit: Math.round(width * 0.66666666)
    readonly property var albumId: rawMessage.media_album_id
    property var albumMessageIds: messageListItem ? messageListItem.messageAlbumMessageIds : []//overlayFlickable.messageAlbumMessageIds
    onAlbumMessageIdsChanged: albumMessages = getMessages() //chatModel.getMessagesForAlbum(messageContent.albumId)
    property var albumMessages: getMessages()//chatModel.getMessagesForAlbum(messageContent.albumId)
    property bool firstLarge: albumMessages.length % 2 !== 0;

    clip: true
    height: defaultExtraContentHeight//(firstLarge ? heightUnit * 0.75 : 0 ) + heightUnit * 0.25 * albumMessageIds.length


    onClicked: {
        if(messageListItem.precalculatedValues.pageIsSelecting) {
            page.toggleMessageSelection(rawMessage);
            return;
        }
        openDetail(-1);
    }
    function getMessages() {
        var msgs = [rawMessage];
        if(messageContent.albumId === '0' || messageContent.albumMessageIds.length < 2) {
            return msgs;
        }
//        var othermsgIds =
        // getMessages from tdlib isn't faster
//        if(rawMessage && rawMessage.chat_id) {
//            var messages = [];
//            return albumMessageIds.map(function(msgId){
//                if(msgId === rawMessage.id) {
//                    return rawMessage;
//                }
//                return tdLibWrapper.getMessage(rawMessage.chat_id, msgId);
//            })
//        }
         chatModel.getMessagesForAlbum(messageContent.albumId, 1).forEach(function(msg){
            msgs.push(msg);
         });
        //
        return msgs; //chatModel.getMessagesForAlbum(messageContent.albumId);
    }

    function openDetail(index) {
        console.log('open detail', index || 0);


        pageStack.push(Qt.resolvedUrl("../../pages/MediaAlbumPage.qml"), {
                           "messages" : albumMessages,
                           "index": index || 0
                       })
    }
    Connections { // TODO: needed?
        target: tdLibWrapper

        onReceivedMessage: {
            if (albumMessageIds.indexOf(messageId)) {
//                albumMessages = getMessages()
            }
        }
    }

    Component {
        id: photoPreviewComponent
        MessagePhoto {
//            width: parent.width
//            height: parent.height
            messageListItem: messageContent.messageListItem
            overlayFlickable: messageContent.overlayFlickable
            rawMessage: albumMessages[modelIndex]
            highlighted: mediaBackgroundItem.highlighted
        }
    }
    Component {
        id: videoPreviewComponent
        Item {
            property bool highlighted: mediaBackgroundItem.highlighted
            anchors.fill: parent
            clip: true
            TDLibThumbnail {
                id: tdLibImage
                width: parent.width //don't use anchors here for easier custom scaling
                height: parent.height
                highlighted: parent.highlighted
                thumbnail: albumMessages[modelIndex].content.video.thumbnail
                minithumbnail: albumMessages[modelIndex].content.video.minithumbnail
            }
            Rectangle {
                anchors {
                    fill: videoIcon
                    leftMargin: -Theme.paddingSmall
                    topMargin: -Theme.paddingSmall
                    bottomMargin: -Theme.paddingSmall
                    rightMargin: -Theme.paddingLarge

                }

                radius: Theme.paddingSmall
                color: Theme.rgba(Theme.overlayBackgroundColor, 0.4)

            }

            Icon {
                id: videoIcon
                source: "image://theme/icon-m-video"
                width: Theme.iconSizeSmall
                height: Theme.iconSizeSmall
                highlighted: parent.highlighted
                anchors {
                    right: parent.right
                    rightMargin: Theme.paddingSmall
                    bottom: parent.bottom
                }
            }
        }
    }

    Flow {
        id: contentGrid
        property int firstWidth: firstLarge ? contentGrid.width : normalWidth
        property int firstHeight: firstLarge ? heightUnit - contentGrid.spacing : normalHeight
        property int normalWidth: (contentGrid.width - contentGrid.spacing) / 2
        property int normalHeight: (heightUnit / 2) - contentGrid.spacing

        anchors.fill: parent
        spacing: Theme.paddingMedium

        Repeater {
            model: albumMessages
            delegate: BackgroundItem {
                id: mediaBackgroundItem
                property bool isLarge: firstLarge && model.index === 0
                width: model.index === 0 ? contentGrid.firstWidth : contentGrid.normalWidth
                height: model.index === 0 ? contentGrid.firstHeight : contentGrid.normalHeight

                readonly property bool isSelected: messageListItem.precalculatedValues.pageIsSelecting && page.selectedMessages.some(function(existingMessage) {
                    return existingMessage.id === albumMessages[index].id
                });
                highlighted: isSelected || down || messageContent.highlighted
                onClicked: {
                    if(messageListItem.precalculatedValues.pageIsSelecting) {
                        page.toggleMessageSelection(albumMessages[index]);
                        return;
                    }

                    openDetail(index);
                }
                onPressAndHold: {
                    page.toggleMessageSelection(albumMessages[index]);
                }

                Loader {
                    anchors.fill: parent
//                    asynchronous: true

                    readonly property int modelIndex: index
                    sourceComponent: albumMessages[index].content["@type"] === 'messageVideo' ? videoPreviewComponent : photoPreviewComponent
                    opacity: status === Loader.Ready
                    Behavior on opacity {FadeAnimator{}}
                }

                /*
                  TODO video:
                    rawMessage.content.video.thumbnail
    TDLibPhoto {
        id: photo
        anchors.fill: parent
        photo: rawMessage.content.photo
        highlighted: parent.highlighted
    }
                  */
                Rectangle {
                    visible: mediaBackgroundItem.isSelected
                    anchors {
                        fill: parent
                    }
                    color: 'transparent'
                    border.color: Theme.highlightColor
                    border.width: Theme.paddingSmall
                }
            }
        }
    }
}
