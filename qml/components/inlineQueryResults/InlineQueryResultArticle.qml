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

    title: Emoji.emojify(model.title || "", titleLable.font.pixelSize)
    description: Emoji.emojify(model.description || "", descriptionLabel.font.pixelSize)
    descriptionLabel {
        maximumLineCount: 3
        wrapMode: extraText.length === 0 ? Text.Wrap : Text.NoWrap
    }

    extraText: model.url || ""
    extraTextLabel.visible: !model.hide_url && extraText.length > 0

    thumbnailFileInformation: model.thumbnail ? model.thumbnail.file : {}

    icon.source: "image://theme/icon-m-link"
    icon.visible: thumbnail.visible && thumbnail.opacity === 0

    thumbnail.visible: model.thumbnail || !!model.url
}
