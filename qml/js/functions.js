/*
    Copyright (C) 2020 Sebastian J. Wolf

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
function getUserName(userInformation) {
    return userInformation.first_name + " " + userInformation.last_name;
}

function getSimpleMessageText(message) {
    if (message.content['@type'] === 'messageText') {
        return message.content.text.text;
    }
    return "?";
}

function getDateTimeElapsed(timestamp) {
    return Format.formatDate(new Date(timestamp * 1000), Formatter.DurationElapsed);
}
