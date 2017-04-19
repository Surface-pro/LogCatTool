#-------------------------------------------------
#
# Project created by QtCreator 2017-01-21T10:09:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LogCatTool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    setting.cpp \
    softwareinfo.cpp \
    connectadb.cpp \
    itemsrange.cpp \
    adbthread.cpp \
    qwaitdialog.cpp \
    aboutsoftware.cpp \
    parsesettingfile.cpp \
    security.cpp \
    register.cpp

HEADERS  += mainwindow.h \
    setting.h \
    softwareinfo.h \
    connectadb.h \
    itemsrange.h \
    adbthread.h \
    qwaitdialog.h \
    aboutsoftware.h \
    parsesettingfile.h \
    security.h \
    register.h

FORMS    += mainwindow.ui \
    setting.ui \
    connectadb.ui \
    aboutsoftware.ui \
    register.ui

RESOURCES += \
    image.qrc
RC_FILE += icon.rc
