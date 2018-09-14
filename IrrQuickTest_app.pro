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
TARGET = IrrQuickTest

CONFIG += sailfishapp
QT += widgets opengl

DEFINES += _IRR_COMPILE_WITH_QGLFUNCTIONS_
DEFINES += _IRR_MEDIA=\\\"/usr/share/$$TARGET/media/\\\"
DEFINES += _APP_SHARE=\\\"/usr/share/$$TARGET/\\\"
DEFINES += _MEDIA_PATH=\\\"/usr/share/$$TARGET/media/\\\"
INCLUDEPATH += libs/irrlicht/include
INCLUDEPATH += libs/irrlicht/source/qt
INCLUDEPATH += libs/irrlicht/source/Irrlicht

LIBS += -L$$OUT_PWD/lib -lirrlicht -lpng -lz
SOURCES += src/IrrQuickTest.cpp \
    src/IrrQuickItem.cpp \
    src/IrrQuickItemV2.cpp \
    src/irrglwidget.cpp \
    src/glwidget.cpp

DISTFILES += qml/IrrQuickTest.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    qml/pages/SecondPage.qml \
    rpm/IrrQuickTest.changes.in \
    rpm/IrrQuickTest.changes.run.in \
    rpm/IrrQuickTest.spec \
    rpm/IrrQuickTest.yaml \
    translations/*.ts \
    IrrQuickTest.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 256x256

# to disable building translations every time, comment out the
# following CONFIG line

# its static
irrlib.files = $$PWD/libs/irrlicht/media $$OUT_PWD/lib
irrlib.path = /usr/share/$$TARGET/
INSTALLS += irrlib

CONFIG += sailfishapp_i18n
# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/IrrQuickTest-de.ts

HEADERS += \
    src/IrrQuickItem.h \
    src/IrrQuickItemV2.h \
    src/irrglwidget.h \
    src/glwidget.h
