# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-fernschreiber

CONFIG += sailfishapp sailfishapp_i18n

PKGCONFIG += nemonotifications-qt5 ngf-qt5

QT += core dbus sql

SOURCES += src/harbour-fernschreiber.cpp \
    src/appsettings.cpp \
    src/chatlistmodel.cpp \
    src/chatmodel.cpp \
    src/dbusadaptor.cpp \
    src/dbusinterface.cpp \
    src/emojisearchworker.cpp \
    src/fernschreiberutils.cpp \
    src/notificationmanager.cpp \
    src/processlauncher.cpp \
    src/stickermanager.cpp \
    src/tdlibreceiver.cpp \
    src/tdlibwrapper.cpp

DISTFILES += qml/harbour-fernschreiber.qml \
    qml/components/AudioPreview.qml \
    qml/components/ChatListViewItem.qml \
    qml/components/DocumentPreview.qml \
    qml/components/ImagePreview.qml \
    qml/components/InReplyToRow.qml \
    qml/components/LocationPreview.qml \
    qml/components/StickerPicker.qml \
    qml/components/PhotoTextsListItem.qml \
    qml/components/WebPagePreview.qml \
    qml/components/chatInformationPage/ChatInformationEditArea.qml \
    qml/components/chatInformationPage/ChatInformationProfilePicture.qml \
    qml/components/chatInformationPage/ChatInformationProfilePictureList.qml \
    qml/components/chatInformationPage/ChatInformationTabItemBase.qml \
    qml/components/chatInformationPage/ChatInformationTabItemDebug.qml \
    qml/components/chatInformationPage/ChatInformationTabItemMembersGroups.qml \
    qml/components/chatInformationPage/ChatInformationTabItemSettings.qml \
    qml/components/chatInformationPage/ChatInformationTabView.qml \
    qml/components/chatInformationPage/ChatInformationTextItem.qml \
    qml/components/chatInformationPage/EditGroupChatPermissionsColumn.qml \
    qml/components/chatInformationPage/EditSuperGroupSlowModeColumn.qml \
    qml/js/functions.js \
    qml/pages/ChatInformationPage.qml \
    qml/pages/ChatPage.qml \
    qml/pages/CoverPage.qml \
    qml/pages/InitializationPage.qml \
    qml/pages/OverviewPage.qml \
    qml/pages/AboutPage.qml \
    qml/pages/SettingsPage.qml \
    qml/pages/VideoPage.qml \
    rpm/harbour-fernschreiber.changes.in \
    rpm/harbour-fernschreiber.spec \
    rpm/harbour-fernschreiber.yaml \
    translations/*.ts \
    harbour-fernschreiber.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172 256x256

TRANSLATIONS += translations/harbour-fernschreiber-de.ts \
                translations/harbour-fernschreiber-es.ts \
                translations/harbour-fernschreiber-fi.ts \
                translations/harbour-fernschreiber-hu.ts \
                translations/harbour-fernschreiber-it.ts \
                translations/harbour-fernschreiber-pl.ts \
                translations/harbour-fernschreiber-ru.ts \
                translations/harbour-fernschreiber-sv.ts \
                translations/harbour-fernschreiber-zh_CN.ts

contains(QT_ARCH, arm) {
    TARGET_ARCHITECTURE = armv7hl
} else {
    TARGET_ARCHITECTURE = i486
}

INCLUDEPATH += $$PWD/tdlib/include
DEPENDPATH += $$PWD/tdlib/include
LIBS += -L$$PWD/tdlib/$${TARGET_ARCHITECTURE}/lib/ -ltdjson
telegram.files = $$PWD/tdlib/$${TARGET_ARCHITECTURE}/lib
telegram.path = /usr/share/$${TARGET}

gui.files = qml
gui.path = /usr/share/$${TARGET}

images.files = images
images.path = /usr/share/$${TARGET}

ICONPATH = /usr/share/icons/hicolor

86.png.path = $${ICONPATH}/86x86/apps/
86.png.files += icons/86x86/harbour-fernschreiber.png

108.png.path = $${ICONPATH}/108x108/apps/
108.png.files += icons/108x108/harbour-fernschreiber.png

128.png.path = $${ICONPATH}/128x128/apps/
128.png.files += icons/128x128/harbour-fernschreiber.png

172.png.path = $${ICONPATH}/172x172/apps/
172.png.files += icons/172x172/harbour-fernschreiber.png

256.png.path = $${ICONPATH}/256x256/apps/
256.png.files += icons/256x256/harbour-fernschreiber.png

fernschreiber.desktop.path = /usr/share/applications/
fernschreiber.desktop.files = harbour-fernschreiber.desktop

database.files = db
database.path = /usr/share/$${TARGET}

INSTALLS += telegram 86.png 108.png 128.png 172.png 256.png \
            fernschreiber.desktop gui images database

HEADERS += \
    src/appsettings.h \
    src/chatlistmodel.h \
    src/chatmodel.h \
    src/dbusadaptor.h \
    src/dbusinterface.h \
    src/emojisearchworker.h \
    src/fernschreiberutils.h \
    src/notificationmanager.h \
    src/processlauncher.h \
    src/stickermanager.h \
    src/tdlibreceiver.h \
    src/tdlibsecrets.h \
    src/tdlibwrapper.h
