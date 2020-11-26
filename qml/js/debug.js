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

.pragma library
.import WerkWolf.Fernschreiber 1.0 as Fernschreiber
var enabled = Fernschreiber.DebugLog.enabled;
var log = enabled ? console.log : function(){};
var assert = enabled ? console.assert : function(){};
var time = enabled ? console.time : function(){};
var timeEnd = enabled ? console.timeEnd : function(){};
var trace = enabled ? console.trace : function(){};
var count = enabled ? console.count : function(){};
var profile = enabled ? console.profile : function(){};
var profileEnd = enabled ? console.profileEnd : function(){};
var exception = enabled ? console.exception : function(){};
var warn = enabled ? console.warn: function(){};
// Debug.error is always enabled.
var error = console.error;

Fernschreiber.DebugLog.enabledChanged.connect(function() {
    enabled = Fernschreiber.DebugLog.enabled;
    if(enabled) {
        log = console.log;
        assert = console.assert;
        time = console.time;
        timeEnd = console.timeEnd;
        trace = console.trace;
        count = console.count;
        profile = console.profile;
        profileEnd = console.profileEnd;
        exception = console.exception;
    } else {
        log = assert = time = timeEnd = trace = count = profile = profileEnd = exception = warn = function(){};
    }

});
