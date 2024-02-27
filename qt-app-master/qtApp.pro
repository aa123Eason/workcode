QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addrcdevice.cpp \
    comboboxselectdlg.cpp \
    devadd.cpp \
    devedit.cpp \
    dialogdevprop.cpp \
    dialogproto.cpp \
    editrcdevice.cpp \
    facedit.cpp \
    factoradd.cpp \
    httpclinet.cpp \
    main.cpp \
    mainwindow.cpp \
    rclogdlg.cpp \
    teshuzhiadd.cpp \
    teshuzhidele.cpp \
    upsetadd.cpp \
    usbupdatedlg.cpp \
    util.cpp\
    uartThread/posix_qextserialport.cpp \
    uartThread/qextserialbase.cpp \
    uartThread/uartthread.cpp \
    devicecmdctrldlg.cpp \
    serial.cpp \
    serialport.cpp \
    selectdtdlg.cpp

HEADERS += \
    addrcdevice.h \
    comboboxselectdlg.h \
    common.h \
    devadd.h \
    devedit.h \
    dialogdevprop.h \
    dialogproto.h \
    editrcdevice.h \
    facedit.h \
    factoradd.h \
    httpclinet.h \
    mainwindow.h \
    rclogdlg.h \
    teshuzhiadd.h \
    teshuzhidele.h \
    upsetadd.h \
    usbupdatedlg.h \
    util.h \
    uartThread/posix_qextserialport.h \
    uartThread/qextserialbase.h \
    uartThread/uartthread.h \
    devicecmdctrldlg.h \
    serial.h \
    serialport.h \
    selectdtdlg.h

FORMS += \
    addrcdevice.ui \
    comboboxselectdlg.ui \
    devadd.ui \
    devedit.ui \
    dialogdevprop.ui \
    dialogproto.ui \
    editrcdevice.ui \
    facedit.ui \
    factoradd.ui \
    mainwindow.ui \
    rclogdlg.ui \
    teshuzhiadd.ui \
    teshuzhidele.ui \
    upsetadd.ui \
    devicecmdctrldlg.ui \
    usbupdatedlg.ui \
    selectdtdlg.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

include(src/xlsx/qtxlsx.pri)
include(QsLog/QsLog.pri)

TRANSLATIONS += \
    qtApp_zh_CN.ts
