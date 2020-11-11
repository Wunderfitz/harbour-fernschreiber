# Fernschreiber
A Telegram client for Sailfish OS

## Authors
Sebastian J. Wolf [sebastian@ygriega.de](mailto:sebastian@ygriega.de) and several contributors

## Contributions

Fernschreiber wouldn't be the same without all the people helping in making it better. Thank you very much to all contributors!

### Code (Features, Bugfixes, Optimizations etc.)
- Chat list model, notifications, TDLib receiver, animated stickers, project dependencies: [Slava Monich](https://github.com/monich)
- Chat info page, performance improvements to chat page, location support, app initialization/registration with Telegram, project dependencies: [jgibbon](https://github.com/jgibbon)
- Copy message to clipboard [Christian Stemmle](https://github.com/chstem)

### Logo/Icon
- Designed by [Matteo](https://github.com/iamnomeutente)

### Translations
- Chinese: [dashinfantry](https://github.com/dashinfantry)
- Finnish: [jorm1s](https://github.com/jorm1s)
- Hungarian: [edp17](https://github.com/edp17)
- Italian: [Matteo](https://github.com/iamnomeutente)
- Polish: [atlochowski](https://github.com/atlochowski)
- Russian: [Rustem Abzalov](https://github.com/arustg) and [Slava Monich](https://github.com/monich)
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

Moreover, you need to have a compiled version of [TDLib](https://github.com/tdlib/td) in the sub-directory `tdlib`. This sub-directory must contain another sub-directory that fits to the target device architecture (e.g. armv7hl, i486). Within this directory, there needs to be a folder called `lib` that contains at least `libtdjson.so`. For armv7hl the relative path would consequently be `tdlib/armv7hl/lib`.

If you want to contribute bug fixes, improvements, new features etc. please create a pull request (PR). PRs are always welcome and will be reviewed as soon as possible, but may take some time. :)

## Credits

This project uses

- The Telegram Database Library (TDLib) - available on [GitHub.com](https://github.com/tdlib/td). Thanks for making it available under the conditions of the Boost Software License 1.0! Details about the license of TDLib in [its license file](https://github.com/tdlib/td/blob/master/LICENSE_1_0.txt).
- Emoji parsing and artwork by [Twitter Emoji (Twemoji)](http://twitter.github.io/twemoji/), copyright 2018 Twitter, Inc and other contributors, Code licensed under the [MIT License](http://opensource.org/licenses/MIT), Graphics licensed under [CC-BY 4.0](https://creativecommons.org/licenses/by/4.0/)
- Animated sticker parsing and animation by [rlottie](https://github.com/Samsung/rlottie), copyright 2020 Samsung Electronics Co., Ltd. and [other contributors](https://github.com/Samsung/rlottie/blob/master/AUTHORS), Code licensed under the [MIT License](https://github.com/Samsung/rlottie/blob/master/licenses/COPYING.MIT), some rlottie components [licensed under other licenses](https://github.com/Samsung/rlottie/blob/master/COPYING).

Thanks to the maintainers of the used components and - again - all contributors to Fernschreiber!
