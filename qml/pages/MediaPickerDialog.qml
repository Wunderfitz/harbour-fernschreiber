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
import QtDocGallery 5.0
import Sailfish.Silica 1.0
import Nemo.Thumbnailer 1.0

// Photos and videos in one grid, newest first, so that both can go into one album.
// The stock pickers only show one kind at a time.
Dialog {
    id: mediaPickerDialog

    property bool showImages: true
    property bool showVideos: true

    // In the order they were picked, which is the order of the album
    property var selectedFiles: []
    property var selectedIndexes: ({})

    canAccept: selectedFiles.length > 0

    function toggleSelected(file) {
        var files = selectedFiles.slice();
        var index = selectedIndexes.hasOwnProperty(file.filePath) ? selectedIndexes[file.filePath] : -1;
        if (index >= 0) {
            files.splice(index, 1);
        } else {
            files.push(file);
        }
        var indexes = {};
        for (var i = 0; i < files.length; i++) {
            indexes[files[i].filePath] = i;
        }
        selectedFiles = files;
        selectedIndexes = indexes;
    }

    function galleryItems(galleryModel, contentType) {
        var items = [];
        for (var i = 0; i < galleryModel.count; i++) {
            var item = galleryModel.get(i);
            items.push({
                "filePath": item.filePath,
                "fileName": item.fileName,
                // A QUrl would end up as an object in the ListModel
                "url": "" + item.url,
                "mimeType": item.mimeType,
                "contentType": contentType,
                "duration": item.duration || 0,
                "lastModified": item.lastModified ? item.lastModified.getTime() : 0
            });
        }
        return items;
    }

    function updateMediaModel() {
        var images = showImages ? galleryItems(imageGalleryModel, "photo") : [];
        var videos = showVideos ? galleryItems(videoGalleryModel, "video") : [];
        var items = images.concat(videos);
        items.sort(function(a, b) {
            return b.lastModified - a.lastModified;
        });
        mediaModel.clear();
        for (var i = 0; i < items.length; i++) {
            mediaModel.append(items[i]);
        }
    }

    ListModel {
        id: mediaModel
    }

    // Both models report in pieces, the grid is rebuilt once they settle
    Timer {
        id: updateTimer
        interval: 100
        onTriggered: updateMediaModel()
    }

    DocumentGalleryModel {
        id: imageGalleryModel
        rootType: DocumentGallery.Image
        autoUpdate: true
        properties: [ "url", "filePath", "fileName", "mimeType", "lastModified" ]
        sortProperties: [ "-lastModified" ]
        // Album covers, like the gallery leaves them out
        filter: GalleryStartsWithFilter {
            property: "filePath"
            value: StandardPaths.music
            negated: true
        }
        onCountChanged: updateTimer.restart()
    }

    DocumentGalleryModel {
        id: videoGalleryModel
        rootType: DocumentGallery.Video
        autoUpdate: true
        properties: [ "url", "filePath", "fileName", "mimeType", "lastModified", "duration" ]
        sortProperties: [ "-lastModified" ]
        onCountChanged: updateTimer.restart()
    }

    SilicaGridView {
        id: mediaGridView

        readonly property int columnCount: Math.max(3, Math.floor(width / (Theme.pixelRatio * 180)))

        anchors.fill: parent
        cellWidth: width / columnCount
        cellHeight: cellWidth
        model: mediaModel

        header: DialogHeader {
            title: mediaPickerDialog.selectedFiles.length > 0
                   ? qsTr("%Ln selected", "", mediaPickerDialog.selectedFiles.length)
                   : (mediaPickerDialog.showImages && mediaPickerDialog.showVideos
                      ? qsTr("Photos and videos")
                      : (mediaPickerDialog.showImages ? qsTr("Photos") : qsTr("Videos")))
        }

        delegate: BackgroundItem {
            id: mediaItem

            readonly property int selectionIndex: mediaPickerDialog.selectedIndexes.hasOwnProperty(model.filePath)
                                                  ? mediaPickerDialog.selectedIndexes[model.filePath] : -1

            width: mediaGridView.cellWidth
            height: mediaGridView.cellHeight
            highlighted: down || selectionIndex >= 0

            onClicked: mediaPickerDialog.toggleSelected({
                "filePath": model.filePath,
                "fileName": model.fileName,
                "url": model.url,
                "mimeType": model.mimeType,
                "contentType": model.contentType
            })

            Thumbnail {
                anchors.fill: parent
                sourceSize.width: width
                sourceSize.height: height
                fillMode: Thumbnail.PreserveAspectCrop
                mimeType: model.mimeType
                source: model.url
            }

            Rectangle {
                anchors.fill: parent
                color: Theme.highlightBackgroundColor
                opacity: mediaItem.highlighted ? Theme.highlightBackgroundOpacity : 0
            }

            Rectangle {
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                height: videoDurationLabel.height + 2 * Theme.paddingSmall
                visible: model.contentType === "video"
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "transparent" }
                    GradientStop { position: 1.0; color: Theme.rgba("black", 0.6) }
                }

                Icon {
                    id: videoIcon
                    anchors {
                        left: parent.left
                        leftMargin: Theme.paddingSmall
                        verticalCenter: videoDurationLabel.verticalCenter
                    }
                    width: Theme.iconSizeSmall
                    height: Theme.iconSizeSmall
                    sourceSize.width: width
                    sourceSize.height: height
                    source: "image://theme/icon-m-video?white"
                }

                Label {
                    id: videoDurationLabel
                    anchors {
                        right: parent.right
                        rightMargin: Theme.paddingSmall
                        bottom: parent.bottom
                        bottomMargin: Theme.paddingSmall
                    }
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: "white"
                    text: model.duration > 0 ? Format.formatDuration(model.duration, model.duration >= 3600 ? Formatter.DurationLong : Formatter.DurationShort) : ""
                }
            }

            // Where it goes in the album
            Rectangle {
                anchors {
                    top: parent.top
                    right: parent.right
                    margins: Theme.paddingSmall
                }
                width: Theme.iconSizeSmall
                height: width
                radius: width / 2
                visible: mediaItem.selectionIndex >= 0
                color: Theme.highlightBackgroundColor

                Label {
                    anchors.centerIn: parent
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.primaryColor
                    text: mediaItem.selectionIndex + 1
                }
            }
        }

        ViewPlaceholder {
            enabled: mediaModel.count === 0 && !updateTimer.running
                     && imageGalleryModel.status !== DocumentGalleryModel.Active
                     && videoGalleryModel.status !== DocumentGalleryModel.Active
            text: qsTr("No photos or videos on this device")
        }

        VerticalScrollDecorator {}
    }
}
