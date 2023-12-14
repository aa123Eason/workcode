QT       += core gui charts sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

include(QsLog/QsLog.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Crc16Class.cpp \
    chart.cpp \
    factorinfo.cpp \
    hj212.cpp \
    main.cpp \
    mainwindow.cpp \
    msgbox.cpp \
    paramset.cpp \
    qextserialbase.cpp \
    win_qextserialport.cpp

HEADERS += \
    Crc16Class.h \
    chart.h \
    common.h \
    factorinfo.h \
    hj212.h \
    mainwindow.h \
    msgbox.h \
    paramset.h \
    qextserialbase.h \
    win_qextserialport.h

FORMS += \
    mainwindow.ui \
    paramset.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES +=


include(src/xlsx/qtxlsx.pri)
