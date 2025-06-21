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

TARGET = com.me.legoru.ColorTubes

CONFIG += \
    auroraapp

PKGCONFIG += \

SOURCES += \
    core/boardmodel.cpp \
    core/jctlformat.cpp \
    core/moveitem.cpp \
    core/options.cpp \
    core/solver.cpp \
    core/tubemodel.cpp \
    core/usedcolors.cpp \
    gui/arrowitem.cpp \
    gui/bottlelayer.cpp \
    gui/colorslayer.cpp \
    gui/corklayer.cpp \
    gui/flowerlayer.cpp \
    gui/gameboard.cpp \
    gui/shadelayer.cpp \
    gui/tubeitem.cpp \
    src/com.me.legoru.ColorTubes.cpp \
    src/ctglobal.cpp \
    src/ctimages.cpp \
    src/ctio.cpp \
    src/ctpalette.cpp \
    src/game.cpp

DISTFILES += \
    qml/pages/SettingsPage.qml \
    rpm/com.me.legoru.ColorTubes.spec \
    qml/com.me.legoru.ColorTubes.qml \
    qml/cover/CoverPage.qml \
    qml/items/AboutPanel.qml \
    qml/items/CongratsPanel.qml \
    qml/items/IconButtonItem.qml \
    qml/items/MessagePanel.qml \
    qml/pages/MainPage.qml \
    translations/*.ts \
    com.me.legoru.ColorTubes.desktop

AURORAAPP_ICONS = 86x86 108x108 128x128 172x172

CONFIG += \
    auroraapp_i18n

TRANSLATIONS += \
    translations/com.me.legoru.ColorTubes.ts \
    translations/com.me.legoru.ColorTubes-ru.ts \ 
    translations/com.me.legoru.ColorTubes-tr.ts 

HEADERS += \
    core/boardmodel.h \
    core/jctlformat.h \
    core/moveitem.h \
    core/options.h \
    core/solver.h \
    core/tubemodel.h \
    core/usedcolors.h \
    gui/arrowitem.h \
    gui/bottlelayer.h \
    gui/colorslayer.h \
    gui/corklayer.h \
    gui/flowerlayer.h \
    gui/gameboard.h \
    gui/shadelayer.h \
    gui/tubeitem.h \
    src/ctglobal.h \
    src/ctimages.h \
    src/ctio.h \
    src/ctpalette.h \
    src/game.h

RESOURCES += \
    com.me.legoru.ColorTubes.qrc

QT += \
    core \
    svg \
    quick

DEFINES += SAILFISH_OS

APP_VERSION=$$VERSION-$$RELEASE
unix:DEFINES += APP_VERSION=\\\"$$APP_VERSION\\\"
win32:DEFINES += APP_VERSION=\"$$APP_VERSION\"
