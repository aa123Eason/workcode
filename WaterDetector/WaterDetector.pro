QT       += core gui network sql charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

include(QsLog/QsLog.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Crc16Class.cpp \
    cdevice.cpp \
    cfactor.cpp \
    cjsonfile.cpp \
    cuploadsetting.cpp \
    dialognewdev.cpp \
    dialognewfactor.cpp \
    dialogprotoadd.cpp \
    dialogprotocol.cpp \
    hj212.cpp \
    main.cpp \
    mainwindow.cpp \
    msgbox.cpp \
    qextserialbase.cpp \
    win_qextserialport.cpp

HEADERS += \
    Crc16Class.h \
    cdevice.h \
    cfactor.h \
    cjsonfile.h \
    common.h \
    cuploadsetting.h \
    dialognewdev.h \
    dialognewfactor.h \
    dialogprotoadd.h \
    dialogprotocol.h \
    hj212.h \
    mainwindow.h \
    msgbox.h \
    qextserialbase.h \
    win_qextserialport.h

FORMS += \
    dialognewdev.ui \
    dialognewfactor.ui \
    dialogprotoadd.ui \
    dialogprotocol.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
