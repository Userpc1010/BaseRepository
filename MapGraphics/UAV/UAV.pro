#-------------------------------------------------
#
# Project created by QtCreator 2019-04-03T19:43:18
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UAV
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    MissionPlanerMenu.cpp \
    MyTcpServer.cpp \
    Config_Widget.cpp

HEADERS  += MainWindow.h \
    MissionPlanerMenu.h \
    MyTcpServer.h \
    Data.h \
    Config_Widget.h

FORMS    += MainWindow.ui \
    MissionPlanerMenu.ui \
    Config_Widget.ui

#Linkage for MapGraphics shared library
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../MapGraphics/release/ -lMapGraphics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../MapGraphics/debug/ -lMapGraphics
else:unix:!symbian: LIBS += -L$$OUT_PWD/../MapGraphics/ -lMapGraphics

INCLUDEPATH += $$PWD/../MapGraphics
DEPENDPATH += $$PWD/../MapGraphics
