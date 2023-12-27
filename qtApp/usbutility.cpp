#include "usbutility.h"

UsbUtility::UsbUtility(QObject *parent) : QObject(parent)
{

}

UsbUtility::~UsbUtility()
{

}


QString UsbUtility::matchDevName(QString devname)
{
    QMap<QString,QString> map;
    map["COM1"] = "/dev/tty0";
    map["COM2"] = "/dev/tty1";
    map["COM3"] = "/dev/tty2";
    map["COM4"] = "/dev/tty3";
    map["COM5"] = "/dev/tty4";
    map["COM6"] = "/dev/tty5";
    map["COM7"] = "/dev/tty6";
    map["COM8"] = "/dev/tty7";
    map["COM9"] = "/dev/tty8";
    map["COM10"] = "/dev/tty9";
    map["COM11"] = "/dev/tty10";
    map["COM12"] = "/dev/tty11";
    map["USB-COM1"] = "/dev/ttyUSB0";
    map["USB-COM2"] = "/dev/ttyUSB1";
    map["USB-COM3"] = "/dev/ttyUSB2";
    map["USB-COM4"] = "/dev/ttyUSB3";
    map["XR-COM1"] = "/dev/ttyXR0";
    map["XR-COM2"] = "/dev/ttyXR1";
    map["XR-COM3"] = "/dev/ttyXR2";
    map["XR-COM4"] = "/dev/ttyXR3";

    return map[devname];

}
