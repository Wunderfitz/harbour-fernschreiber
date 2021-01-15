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
import "../../js/twemoji.js" as Emoji

InlineQueryResultDefaultBase {
    id: queryResultItem
    property string locationId
    property var resultData: model.venue

    title: Emoji.emojify(queryResultItem.resultData.title || (queryResultItem.resultData.location.latitude + ":" + queryResultItem.resultData.location.longitude), titleLable.font.pixelSize)
    description: Emoji.emojify(queryResultItem.resultData.address || "", descriptionLabel.font.pixelSize)
    extraText: Emoji.emojify(queryResultItem.resultData.type || "", extraTextLabel.font.pixelSize)


    Connections {
        target: tdLibWrapper
        onFileUpdated: {
            if(fileInformation["@extra"] === queryResultItem.locationId) {
                thumbnailFileInformation = fileInformation
            }

        }
    }

    Component.onCompleted: {
        var dimensions = [ Math.round(thumbnail.width), Math.round(thumbnail.height)];

        locationId = "location:" + resultData.location.latitude + ":" + resultData.location.longitude + ":" + dimensions[0] + ":" + dimensions[1];

        tdLibWrapper.getMapThumbnailFile(chatId, resultData.location.latitude, resultData.location.longitude, dimensions[0], dimensions[1], locationId);
    }
}
