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

CONFIG += \
    sailfishapp

SOURCES += src/ColorTubes.cpp \
    core/boardmodel.cpp \
    core/jctlformat.cpp \
    core/moveitem.cpp \
    core/tubemodel.cpp \
    core/usedcolors.cpp \
    gui/bottlelayer.cpp \
    gui/colorslayer.cpp \
    gui/corklayer.cpp \
    gui/old_tubeflyer.cpp \
    gui/shadelayer.cpp \
    gui/tubeflyer.cpp \
    src/ctglobal.cpp \
    src/io.cpp \
    src/game.cpp \
    src/palette.cpp  \
    src/tubeimages.cpp

DISTFILES += qml/ColorTubes.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    qml/pages/MainPage.qml \
    qml/pages/SecondPage.qml \
    rpm/ColorTubes.changes.in \
    rpm/ColorTubes.changes.run.in \
    rpm/ColorTubes.spec \
    rpm/ColorTubes.yaml \
    translations/*.ts \
    ColorTubes.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += \
    translations/ColorTubes-ru.ts

HEADERS += \
    core/boardmodel.h \
    core/jctlformat.h \
    core/moveitem.h \
    core/tubemodel.h \
    core/usedcolors.h \
    gui/bottlelayer.h \
    gui/colorslayer.h \
    gui/corklayer.h \
    gui/old_tubeflyer.h \
    gui/shadelayer.h \
    gui/tubeflyer.h \
    src/ctglobal.h \
    src/io.h \
    src/game.h \
    src/palette.h \
    src/tubeimages.h

RESOURCES += \
    ColorTubes.qrc

QT += \
    core \
    svg \
    quick
