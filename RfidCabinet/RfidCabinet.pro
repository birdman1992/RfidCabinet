#-------------------------------------------------
#
# Project created by QtCreator 2017-12-11T15:41:35
#
#-------------------------------------------------

QT       += core gui network
target.path = /home
INSTALLS += target

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
    casepanel.cpp \
    Structs/userinfo.cpp \
    Structs/goodslist.cpp \
    Structs/goodscar.cpp \
    Structs/caseaddress.cpp \
    Structs/cabinetinfo.cpp \
    RfidCell/rfidarea.cpp \
    device/devicemanager.cpp \
    device/devicesimulator.cpp \
    device/Qextserial/qextserialport.cpp \
    device/Qextserial/qextserialport_unix.cpp \
    ber_test/src/hash.c \
    device/rfiddevice.cpp \
    ber_test/src/RfidApi.c \
    ber_test/src/profile.c \
    Server/httpapi.cpp \
    Json/cJSON.c \
    device/repertorymanager.cpp \
    Structs/rfidchangeinfo.cpp \
    RfidCell/cabinet.cpp \
    device/rfidant.cpp \
    device/rfidreader.cpp \
    manager/epcmanager.cpp

HEADERS += \
        widget.h \
    rfidwidget.h \
    casepanel.h \
    Structs/userinfo.h \
    Structs/goodslist.h \
    Structs/goodscar.h \
    Structs/caseaddress.h \
    Structs/cabinetinfo.h \
    RfidCell/rfidarea.h \
    config.h \
    device/devicemanager.h \
    device/devicesimulator.h \
    device/Qextserial/qextserialport.h \
    device/Qextserial/qextserialport_p.h \
    device/Qextserial/qextserialport_global.h \
    ber_test/inc/profile.h \
    ber_test/inc/hash.h \
    device/rfiddevice.h \
    ber_test/inc/RfidApi.h \
    Server/httpapi.h \
    Json/cJSON.h \
    device/repertorymanager.h \
    Structs/rfidchangeinfo.h \
    RfidCell/cabinet.h \
    device/rfidant.h \
    device/rfidreader.h \
    manager/epcmanager.h

FORMS += \
        widget.ui \
    rfidwidget.ui \
    casepanel.ui \
    RfidCell/rfidarea.ui \
    device/devicesimulator.ui

RESOURCES += \
    image.qrc

QMAKE_CFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable
QMAKE_CXXFLAGS += -Wno-unused-variable
