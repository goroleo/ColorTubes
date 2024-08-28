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
TARGET = ColorTubes
#TARGET = com.me.legoru.ColorTubes

CONFIG += \
    sailfishapp \
    sailfishapp_i18n

SOURCES += \
    src/ColorTubes.cpp \
    core/boardmodel.cpp \
    core/jctlformat.cpp \
    core/levelsengine.cpp \
    core/moveitem.cpp \
    core/tubemodel.cpp \
    core/usedcolors.cpp \
    gui/bottlelayer.cpp \
    gui/colorslayer.cpp \
    gui/corklayer.cpp \
    gui/flowerlayer.cpp \
    gui/gameboard.cpp \
    gui/shadelayer.cpp \
    gui/tubeitem.cpp \
    src/ctglobal.cpp \
    src/ctimages.cpp \
    src/ctio.cpp \
    src/ctpalette.cpp \
    src/game.cpp

DISTFILES += \
    qml/ColorTubes.qml \
    qml/cover/CoverPage.qml \
    qml/items/CongratsPanel.qml \
    qml/items/IconButtonItem.qml \
    qml/items/MessagePanel.qml \
    qml/pages/MainPage.qml \
    rpm/ColorTubes.changes.in \
    rpm/ColorTubes.changes.run.in \
    rpm/ColorTubes.spec \
    rpm/ColorTubes.yaml \
    translations/*.ts \
    ColorTubes.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

TRANSLATIONS += \
    translations/ColorTubes.ts \
    translations/ColorTubes-ru.ts

HEADERS += \
    core/boardmodel.h \
    core/jctlformat.h \
    core/levelsengine.h \
    core/moveitem.h \
    core/tubemodel.h \
    core/usedcolors.h \
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
    ColorTubes.qrc

QT += \
    core \
    svg \
    quick \
    widgets

