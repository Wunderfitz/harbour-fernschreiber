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

/**
 * @function compareAndRepeat
 * This function compares results of two functions for multiple sets of arguments and then repeats  those calls to compare them with profiling.
 * Testing showed that results are slightly skewed in favor of the secondaryProfileFunction, which (together with external factors)
 * is almost compensated by running the calls often.
 *
 * @param {string} title - used for Debug output
 * @param {function} profileFunction - function to compare
 * @param {function} [secondaryProfileFunction] - secondary function to compare. Use falsy value to skip comparison.
 * @param {Array.<Array.<*>>} [functionArgumentsArray = []] - argument sets to run both functions with
 * @param {number} [testCount = 10000] - times to run each function in addition to result comparison
 * @param {Object} [testContext = null] - If the functions use `this` or otherwise depend on a Closure/Execution context, it can be set here. If they, for example, use properties of a QML `Item`, you can use that `Item` as `testContext`.
 *
 * @example
 * // to compare the results of Debug.log("first", "second", "third")
 * // and Debug.log("some value")
 * // with the same calls of console.log and run 100 times each:
 * Debug.compareAndRepeat("Debuglog",
 *   Debug.log, console.log,
 *   [["first", "second", "third"], ["some value"]],
 *   100
 * );
 *
*/

function compareAndRepeat(title, profileFunction, secondaryProfileFunction, functionArgumentsArray, testCount, testContext) {
    if(!enabled) {
        log("Debugging disabled, compareAndRepeat ran uselessly:", title);
        return;
    }
    var numberOfTests = testCount || 10000,
    context = testContext || null,
    args = functionArgumentsArray || [],
    argumentIterator  = args.length || 1,
    results,
    resultsComparison,
    functionIterator = numberOfTests;

    if(secondaryProfileFunction) {
        while(argumentIterator--) {
            results = JSON.stringify(profileFunction.apply(context, args[argumentIterator] || null));
            resultsComparison = JSON.stringify(secondaryProfileFunction.apply(context, args[argumentIterator] || null));
            if(resultsComparison !== results) {
                warn("\x1b[1m✘ Different return values!\x1b[0m", title)
                warn("profileFunction result:", results);
                warn("secondaryProfileFunction result:", resultsComparison);
                return;
            }
        }
        log("\x1b[1m✔ Comparison of", title, "return values successful!\x1b[0m")
    }
    log("Running", title, "with", args.length, "different argument sets", numberOfTests, "times each")

    if(secondaryProfileFunction) {
        // "secondaryProfileFunction"
        functionIterator = numberOfTests;

        while(functionIterator--) {
            argumentIterator = args.length || 1;
            while(argumentIterator--) {
                secondaryProfileFunction.apply(context, args[argumentIterator] || null);
            }
        }
    }

    // "profileFunction"
    functionIterator = numberOfTests;
    while(functionIterator--) {
        argumentIterator = args.length || 1;
        while(argumentIterator--) {
            profileFunction.apply(context, args[argumentIterator] || null);
        }
    }

    log("Ran", title, args.length * numberOfTests, "times", secondaryProfileFunction ? "and compared results successfully" : "");

}
