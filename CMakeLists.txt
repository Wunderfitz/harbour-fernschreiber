project(harbour-fernschreiber CXX)
cmake_minimum_required(VERSION 3.5)

find_package(Qt5 COMPONENTS Core Network Qml Gui Quick Sql Positioning DBus Multimedia REQUIRED)

include(FindPkgConfig)
pkg_search_module(SAILFISH sailfishapp REQUIRED)

pkg_search_module(NEMONOTIFICATIONS-QT5 nemonotifications-qt5 REQUIRED IMPORTED_TARGET)

find_package(ZLIB REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(td-src)

option(LOTTIE_MODULE "Enable LOTTIE MODULE SUPPORT" OFF)
add_subdirectory(rlottie)

set(harbour-fernschreiber_SRCS src/harbour-fernschreiber.cpp
    src/appsettings.cpp
    src/chatlistmodel.cpp
    src/chatmodel.cpp
    src/contactsmodel.cpp
    src/dbusadaptor.cpp
    src/dbusinterface.cpp
    src/emojisearchworker.cpp
    src/fernschreiberutils.cpp
    src/knownusersmodel.cpp
    src/mceinterface.cpp
    src/namedaction.cpp
    src/notificationmanager.cpp
    src/processlauncher.cpp
    src/stickermanager.cpp
    src/tdlibfile.cpp
    src/tdlibreceiver.cpp
    src/tdlibwrapper.cpp
    src/tgsplugin.cpp
)

set(harbour-fernschreiber_HDRS
    src/debuglog.h
    src/debuglogjs.h
)

add_executable(harbour-fernschreiber ${harbour-fernschreiber_SRCS} ${harbour-fernschreiber_HDRS})
target_include_directories(harbour-fernschreiber PRIVATE
    $<BUILD_INTERFACE:
    ${SAILFISH_INCLUDE_DIRS}>
    tdlib/include
)
target_link_libraries(harbour-fernschreiber
    Td::TdJson
    Td::TdStatic
    rlottie::rlottie
    Qt5::DBus
    Qt5::Multimedia
    Qt5::Positioning
    Qt5::Quick
    Qt5::Sql
    ZLIB::ZLIB
    PkgConfig::NEMONOTIFICATIONS-QT5
    ${SAILFISH_LDFLAGS}
)

target_compile_definitions(harbour-fernschreiber PRIVATE QT_STATICPLUGIN)

install(TARGETS harbour-fernschreiber
    RUNTIME DESTINATION bin
)
install(DIRECTORY qml images db
    DESTINATION share/harbour-fernschreiber
)
install(DIRECTORY translations
    DESTINATION share/harbour-fernschreiber
    FILES_MATCHING PATTERN "*.qm"
)
install(FILES harbour-fernschreiber.desktop
    DESTINATION share/applications
)

install(FILES icons/86x86/harbour-fernschreiber.png
    DESTINATION share/icons/hicolor/86x86/apps
)
install(FILES icons/108x108/harbour-fernschreiber.png
    DESTINATION share/icons/hicolor/108x108/apps
)
install(FILES icons/128x128/harbour-fernschreiber.png
    DESTINATION share/icons/hicolor/128x128/apps
)
install(FILES icons/172x172/harbour-fernschreiber.png
    DESTINATION share/icons/hicolor/172x172/apps
)
install(FILES icons/256x256/harbour-fernschreiber.png
    DESTINATION share/icons/hicolor/256x256/apps
)


# Tell Qt Creator where the application executable(s) would be located on the
# device.
#
# It is not necessary to list other deployables than executables (runtime
# targets) here. The deployment process of Sailfish OS projects is opaque to
# Qt Creator and the information contained in QtCreatorDeployment.txt is only
# used to locate the executable associated with the active run configuration
# on the device in order to run it.
#
# Search the Qt Creator Manual to learn about the QtCreatorDeployment.txt file
# format.
file(WRITE "${CMAKE_BINARY_DIR}/QtCreatorDeployment.txt"
    "${CMAKE_INSTALL_PREFIX}\n${CMAKE_BINARY_DIR}/harbour-fernschreiber:bin\n")
