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
import Nemo.Thumbnailer 1.0

Item {
    id: tdlibThumbnail
    /*
        Optional thumbnail, usually as property "thumbnail".
        The following TDLib objects can have it:
            - animation
            - audio (as "album_cover_thumbnail")
            - document
            - sticker (no minithumbnail)
            - video
            - videoNote
            - stickerSet (no minithumbnail)
            - stickerSetInfo (no minithumbnail)
            - inlineQueryResultArticle (no minithumbnail)
            - inlineQueryResultContact (no minithumbnail)
            - inlineQueryResultLocation (no minithumbnail)
            - inlineQueryResultVenue (no minithumbnail)
    */
    property var thumbnail
    /*
        Optional minithumbnail, usually as property "minithumbnail".
        Has data inline: If present, it doesn't need another request.
        The following TDLib objects can have it:
            - animation
            - audio (as "album_cover_minithumbnail")
            - document
            - photo / chatPhoto (Note: No thumbnail, so not applicable here)
            - video
            - videoNote
    */
    property alias minithumbnail: minithumbnailLoader.minithumbnail
    property bool useBackgroundImage: true
    property bool highlighted

    property bool isVideo: !!thumbnail && thumbnail.format["@type"] === "thumbnailFormatMpeg4"
    property string videoMimeType: "video/mp4"

    readonly property bool hasVisibleThumbnail: thumbnailImage.opacity !== 1.0
        && !(videoThumbnailLoader.item && videoThumbnailLoader.item.opacity === 1.0)
    property alias fillMode: thumbnailImage.fillMode
    layer {
        enabled: highlighted
        effect: PressEffect { source: tdlibThumbnail }
    }

    TDLibMinithumbnail {
        id: minithumbnailLoader
        fillMode: thumbnailImage.fillMode
        active: !!minithumbnail && thumbnailImage.opacity < 1.0
    }
    BackgroundImage {
        visible: tdlibThumbnail.useBackgroundImage && thumbnailImage.opacity < 1.0
    }

    // image thumbnail
    TDLibImage {
        id: thumbnailImage
        anchors.fill: parent
        enabled: !parent.isVideo
        fileInformation: tdlibThumbnail.thumbnail ? tdlibThumbnail.thumbnail.file : {}
        onStatusChanged: { //TODO check if this is really how it is ;)
            if(status === Image.Error) {
                // in some cases, webp is used (without correct mime type).
                // we just try it blindly and cross our fingers:
                tdlibThumbnail.videoMimeType = "image/webp";
                tdlibThumbnail.isVideo = true;
            }
        }
    }

    // Fallback for video thumbnail format: try to use Nemo.Thumbnailer
    Loader {
        id: videoThumbnailLoader
        active: parent.isVideo
        asynchronous: true
        anchors.fill: parent
        sourceComponent: Component {
            id: videoThumbnail
            Thumbnail {
                id: thumbnail
                source: thumbnailImage.file.path
                sourceSize.width: width
                sourceSize.height: height
                mimeType: tdlibThumbnail.videoMimeType
                fillMode: thumbnailImage.fillMode == Image.PreserveAspectFit ? Thumbnail.PreserveAspectFit : Thumbnail.PreserveAspectCrop
                visible: opacity > 0
                opacity: status === Thumbnail.Ready ? 1.0 : 0.0
                Behavior on opacity { FadeAnimation {} }
            }
        }
    }
}
