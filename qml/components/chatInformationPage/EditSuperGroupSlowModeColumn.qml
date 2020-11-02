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


Column {
    SectionHeader {
        text: qsTr("Slow Mode")
    }

    Slider {
        width: parent.width + Theme.horizontalPageMargin * 2
        x: -Theme.horizontalPageMargin
        property var presetValues: [0, 10, 30, 60, 300, 900, 3600]
        property int realValue: chatInformationPage.groupFullInformation.slow_mode_delay
        property int realValueIndex: presetValues.indexOf(realValue) > -1 ? presetValues.indexOf(realValue) : 0
        value: realValueIndex
        minimumValue: 0
        maximumValue: presetValues.length - 1
        stepSize: 1
        valueText: value === 0 ? qsTr("Off") : Format.formatDuration(presetValues[value], presetValues[value] < 3600 ? Formatter.DurationShort : Formatter.DurationLong);
        onPressedChanged: {
            if(!pressed && value !== realValueIndex) {
                tdLibWrapper.setChatSlowModeDelay(chatInformationPage.chatInformation.id, presetValues[value]);
            }
        }
    }
    Label {
        text: qsTr("Set how long every chat member has to wait between Messages")
        width: parent.width - Theme.horizontalPageMargin * 2
        x: Theme.horizontalPageMargin
        font.pixelSize: Theme.fontSizeSmall
        color: Theme.secondaryHighlightColor
        wrapMode: Text.Wrap
    }
}
