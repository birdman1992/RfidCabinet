#-------------------------------------------------
#
# Project created by QtCreator 2017-12-11T15:41:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RfidCabinet
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        widget.cpp \
    rfidwidget.cpp \
    rfidwidget.cpp \
    casepanel.cpp \
    Structs/userinfo.cpp \
    Structs/goodslist.cpp \
    Structs/goodscar.cpp \
    Structs/caseaddress.cpp \
    Structs/cabinetinfo.cpp

HEADERS += \
        widget.h \
    rfidwidget.h \
    rfidwidget.h \
    casepanel.h \
    Structs/userinfo.h \
    Structs/goodslist.h \
    Structs/goodscar.h \
    Structs/caseaddress.h \
    Structs/cabinetinfo.h

FORMS += \
        widget.ui \
    rfidwidget.ui \
    rfidwidget.ui \
    casepanel.ui

RESOURCES += \
    image.qrc
