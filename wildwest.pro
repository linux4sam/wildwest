#-------------------------------------------------
#
# Project created by QtCreator 2018-01-15T17:07:32
#
#-------------------------------------------------

QT       += core gui gui-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wildwest
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS QT_NO_DEBUG_OUTPUT

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp \
    planemanager.cpp \
    tools.cpp \
    graphicsplaneitem.cpp \
    graphicslayeritem.cpp \
    graphicsplaneview.cpp \
    graphicsspriteitem.cpp

HEADERS  += \
    planemanager.h \
    tools.h \
    graphicsplaneitem.h \
    graphicslayeritem.h \
    graphicsplaneview.h \
    graphicsspriteitem.h

DISTFILES += \
    wildwest.screen

target.path = /opt/wildwest
target.files = wildwest
extra.path = /opt/wildwest
extra.files = resources/wildwest.sh wildwest.screen
configfile.path = /opt/ApplicationLauncher/applications/xml
configfile.files = resources/10-wildwest.xml
imagefile.path = /opt/ApplicationLauncher/applications/resources
imagefile.files = resources/wildwest.png
INSTALLS += target configfile imagefile extra

CONFIG += link_pkgconfig

LOCALPLANES {
    PKGCONFIG += tslib
    INCLUDEPATH += $(HOME)/planes/include/
    LIBS += -L$(HOME)/planes/src/.libs -lplanes
} else {
    PKGCONFIG += libplanes
}

PKGCONFIG += libdrm cairo libcjson lua

#CONFIG += LOCALPLANES

RESOURCES += \
    media.qrc
