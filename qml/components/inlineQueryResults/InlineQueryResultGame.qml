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

    title: Emoji.emojify(model.game.title || "", titleLable.font.pixelSize)
    description: Emoji.emojify(model.game.description || "", descriptionLabel.font.pixelSize)
    descriptionLabel {
        maximumLineCount: 3
        wrapMode: Text.Wrap
    }

    icon.source: "image://theme/icon-m-game-controller"
    icon.visible: thumbnail.opacity === 0


    Component.onCompleted: {
        if (model.game.photo) {
            // Check first which size fits best...
            var photo
            for (var i = 0; i < model.game.photo.sizes.length; i++) {
                photo = model.game.photo.sizes[i].photo
                if (model.game.photo.sizes[i].width >= queryResultItem.width) {
                    break
                }
            }
            if (photo) {
                thumbnailFileInformation = photo
            }
        }
    }
}
