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

QT += core dbus

SOURCES += src/harbour-fernschreiber.cpp \
    src/chatlistmodel.cpp \
    src/chatmodel.cpp \
    src/dbusadaptor.cpp \
    src/dbusinterface.cpp \
    src/notificationmanager.cpp \
    src/tdlibreceiver.cpp \
    src/tdlibwrapper.cpp

DISTFILES += qml/harbour-fernschreiber.qml \
    qml/components/AudioPreview.qml \
    qml/components/DocumentPreview.qml \
    qml/components/ImagePreview.qml \
    qml/components/InReplyToRow.qml \
    qml/components/WebPagePreview.qml \
    qml/js/functions.js \
    qml/pages/ChatPage.qml \
    qml/pages/CoverPage.qml \
    qml/pages/InitializationPage.qml \
    qml/pages/OverviewPage.qml \
    qml/pages/AboutPage.qml \
    qml/pages/VideoPage.qml \
    rpm/harbour-fernschreiber.changes.in \
    rpm/harbour-fernschreiber.changes.run.in \
    rpm/harbour-fernschreiber.spec \
    rpm/harbour-fernschreiber.yaml \
    translations/*.ts \
    harbour-fernschreiber.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172 256x256

TRANSLATIONS += translations/harbour-fernschreiber-de.ts

LIBS += -L$$PWD/tdlib/lib/ -ltdjson

INCLUDEPATH += $$PWD/tdlib/include
DEPENDPATH += $$PWD/tdlib/include

telegram.files = $$PWD/tdlib/lib
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

INSTALLS += telegram 86.png 108.png 128.png 172.png 256.png \
            fernschreiber.desktop gui images

HEADERS += \
    src/chatlistmodel.h \
    src/chatmodel.h \
    src/dbusadaptor.h \
    src/dbusinterface.h \
    src/notificationmanager.h \
    src/tdlibreceiver.h \
    src/tdlibsecrets.h \
    src/tdlibwrapper.h
