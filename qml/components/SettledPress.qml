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

/*
    Tells a press apart from the beginning of a scroll.

    A finger landing on a list is more often about to drag it than to press
    what it landed on, so a press effect that follows the touch straight away
    turns every scroll into a trail of flashing items. Feed the press of an
    item or a mouse area in here and "active" only turns true once the finger
    has stayed put long enough for a scroll to be ruled out - a scroll never
    gets that far, since whoever reports the press gives it up as soon as the
    view takes the touch over.
*/
Timer {
    property bool pressed
    readonly property bool active: pressed && !running

    // Long enough to outlast the beginning of a scroll, short enough to stay
    // ahead of the eye of whoever really is pressing
    interval: 150
    repeat: false
    running: pressed
}
