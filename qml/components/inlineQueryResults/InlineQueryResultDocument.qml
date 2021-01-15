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
import "../"
import "../../js/twemoji.js" as Emoji

Loader {
    Component {
        id: documentComponent
        InlineQueryResultDefaultBase {
            id: queryResultItem

            title: Emoji.emojify(model.title || model.document.file_name || "", titleLable.font.pixelSize)
            description: Emoji.emojify(model.description || model.document.file_name || "", descriptionLabel.font.pixelSize)
            extraText: Format.formatFileSize(model.document.document.expected_size)

            thumbnailFileInformation: model.thumbnail ? model.thumbnail.file : {}

            icon.source: Theme.iconForMimeType(model.document.mime_type)
            icon.visible: thumbnail.visible && thumbnail.opacity === 0
        }
    }
    Component {
        id: voiceNoteDocumentComponent
        InlineQueryResultVoiceNote {
            resultData: model.document
            audioData: model.document.document
        }
    }
    sourceComponent: model.document.mime_type === "audio/ogg" ? voiceNoteDocumentComponent : documentComponent
}
