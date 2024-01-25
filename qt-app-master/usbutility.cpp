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
    map["COM1"] = "/dev/ttyS0";
    map["COM2"] = "/dev/ttyS1";
    map["COM3"] = "/dev/ttyS2";
    map["COM4"] = "/dev/ttyS3";
    map["COM5"] = "/dev/ttyS4";
    map["COM6"] = "/dev/ttyS5";
    map["COM7"] = "/dev/ttyS6";
    map["COM8"] = "/dev/ttyS7";
    map["COM9"] = "/dev/ttyS8";
    map["COM10"] = "/dev/ttyS9";
    map["COM11"] = "/dev/ttyS10";
    map["COM12"] = "/dev/ttyS11";
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
