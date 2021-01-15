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
    property string namesSeparator: model.contact.first_name && model.contact.last_name ? " " : ""

    title: Emoji.emojify(model.contact.first_name + namesSeparator + model.contact.last_name || "", titleLable.font.pixelSize)
    description: Emoji.emojify(model.contact.phone_number || "", descriptionLabel.font.pixelSize)

    extraText: model.url || ""
    extraTextLabel.visible: !model.hide_url && extraText.length > 0

    thumbnailFileInformation: model.thumbnail ? model.thumbnail.file : {}

    icon.source: "image://theme/icon-m-contact"
    icon.visible: thumbnail.visible && thumbnail.opacity === 0

}
