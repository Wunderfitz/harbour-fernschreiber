# Fernschreiber
A Telegram client for Sailfish OS

## Authors
Sebastian J. Wolf [sebastian@ygriega.de](mailto:sebastian@ygriega.de) and several contributors

## Contributions

Fernschreiber wouldn't be the same without all the people helping in making it better. Thank you very much to all contributors!

### Code (Features, Bugfixes, Optimizations etc.)
- Chat list model, chat model, notifications, TDLib receiver, animated stickers, project dependencies, qml/c++ optimizations, chatPhoto, TDLibFile, code reviews, logging categories: [Slava Monich](https://github.com/monich)
- Chat info page, performance improvements to chat page, location support, app initialization/registration with Telegram, project dependencies, emoji handling, qml/js optimizations, multi-message actions, i18n fixes, chat permission handling, code reviews, logging categories, bot support: [jgibbon](https://github.com/jgibbon)
- Copy message to clipboard [Christian Stemmle](https://github.com/chstem)
- Hide send message button if send-by-enter is switched on [santhoshmanikandan](https://github.com/santhoshmanikandan)

### Logo/Icon
- Designed by [Matteo](https://github.com/iamnomeutente), adjustments by [Slava Monich](https://github.com/monich)

### Translations
- Chinese: [dashinfantry](https://github.com/dashinfantry)
- Finnish: [jorm1s](https://github.com/jorm1s)
- Hungarian: [edp17](https://github.com/edp17)
- Italian: [Matteo](https://github.com/iamnomeutente)
- Polish: [atlochowski](https://github.com/atlochowski)
- Russian: [Rustem Abzalov](https://github.com/arustg) and [Slava Monich](https://github.com/monich)
- Slovak: [okruhliak](https://github.com/okruhliak)
- Spanish: [carlosgonz](https://github.com/GNUuser)
- Swedish: [Åke Engelbrektson](https://github.com/eson57)

## License
Licensed under GNU GPLv3

## Build
Simply clone this repository and ensure to have all [submodules](https://git-scm.com/docs/git-submodule) imported as well (e.g. by using `git submodule update --init`). Then use the project file `harbour-fernschreiber.pro` to import the sources in your SailfishOS IDE. To build and run Fernschreiber or an application which is based on Fernschreiber, you need to create the file `harbour-fernschreiber/src/tdlibsecrets.h` and enter the required constants in the following format:

```
#ifndef TDLIBSECRETS_H
#define TDLIBSECRETS_H
const char TDLIB_API_ID[] = "42424242";
const char TDLIB_API_HASH[] = "1234567890abcdef1234567890abcdef";
#endif // TDLIBSECRETS_H
```

You get the Telegram API ID and hash as soon as you've registered your own application on [https://my.telegram.org](https://my.telegram.org).

Moreover, you need to have a compiled version of [TDLib 1.7](https://github.com/tdlib/td) in the sub-directory `tdlib`. This sub-directory must contain another sub-directory that fits to the target device architecture (e.g. armv7hl, i486). Within this directory, there needs to be a folder called `lib` that contains at least `libtdjson.so`. For armv7hl the relative path would consequently be `tdlib/armv7hl/lib`.

In case you encounter strange performance issues on startup (several seconds delay, app seems to do nothing), please be sure to [follow the instructions from the respective GitHub issue](https://github.com/tdlib/td/issues/1322), i.e. let TDLib build SQLite with `-DOMIT_MEMLOCK` and be sure to comment the two lines 22558 (`#ifndef OMIT_MEMLOCK`) and 22567 (`#endif`) in the file `sqlite/sqlite/sqlite3.c`.

Moreover, TDLib 1.7 has issues loading some pinned messages in case the message database is used (which is the case in Fernschreiber). [A small patch](https://github.com/tdlib/td/commit/30d912bd4b145afb8d494b307d37645ffa21ec29) is required to make TDLib work properly in all cases. See [the respective TDLib issue](https://github.com/tdlib/td/issues/1343) for more details.

In case you want to use the same codebase which was used to compile the library that is shipped with Fernschreiber, please [check out the fork](https://github.com/Wunderfitz/td), be sure to use the branch `fernschreiber` and compile these sources using the following commands (be sure to have the Sailfish OS build engine running):

- `alias sfdk=~/SailfishOS/bin/sfdk`
- `sfdk config target=SailfishOS-3.3.0.16-armv7hl` (this compiles the sources on SFOS 3.3 and ARM - the target needs to be adjusted according to the running SDK engine and the platform)
- `mkdir build`
- `cd build`
- `sfdk build-shell cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=../tdlib -DTD_ENABLE_LTO=ON ..` (in case of compilation issues, try removing the flag `-DTD_ENABLE_LTO=ON`)
- `sfdk build-shell cmake --build . --target install`

You'll find the compiled library in the directory `td/tdlib`.


## Debug
Fernschreiber does only output a few TDLib messages by default. To get its own debug log messages, you can either run a debug build to see all of them or use the environment variable `QT_LOGGING_RULES` to specify/filter which messages you'd like to see.

Run `QT_LOGGING_RULES="fernschreiber.*=true" harbour-fernschreiber` to see all messages or replace the `*` with specific logging categories. You'll find the logging category inside the corresponding `.cpp` file for backend usage or you can use `JS` to only see frontend messages.

## Contribute

If you want to contribute bug fixes, improvements, new features etc. please create a pull request (PR). PRs are always welcome and will be reviewed as soon as possible, but may take some time. :)

## Credits

This project uses

- The Telegram Database Library (TDLib) - available on [GitHub.com](https://github.com/tdlib/td). Thanks for making it available under the conditions of the Boost Software License 1.0! Details about the license of TDLib in [its license file](https://github.com/tdlib/td/blob/master/LICENSE_1_0.txt).
- Emoji parsing and artwork by [Twitter Emoji (Twemoji)](http://twitter.github.io/twemoji/), copyright 2018 Twitter, Inc and other contributors, Code licensed under the [MIT License](http://opensource.org/licenses/MIT), Graphics licensed under [CC-BY 4.0](https://creativecommons.org/licenses/by/4.0/)
- Animated sticker parsing and animation by [rlottie](https://github.com/Samsung/rlottie), copyright 2020 Samsung Electronics Co., Ltd. and [other contributors](https://github.com/Samsung/rlottie/blob/master/AUTHORS), Code licensed under the [MIT License](https://github.com/Samsung/rlottie/blob/master/licenses/COPYING.MIT), some rlottie components [licensed under other licenses](https://github.com/Samsung/rlottie/blob/master/COPYING).

Thanks to the maintainers of the used components and - again - all contributors to Fernschreiber!
