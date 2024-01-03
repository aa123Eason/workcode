#include "util.h"

Util::Util()
{

}

QString Util::Uart_Convert(QString comboxTex)
{
    if(comboxTex == "COM1")
    {
        return "/dev/ttyS0";
    }
    else if(comboxTex == "COM2")
    {
        return "/dev/ttyS1";
    }
    else if(comboxTex == "COM3")
    {
        return "/dev/ttyS2";
    }
    else if(comboxTex == "COM4")
    {
        return "/dev/ttyS3";
    }
    else if(comboxTex == "COM5")
    {
        return "/dev/ttyS4";
    }
    else if(comboxTex == "COM6")
    {
        return "/dev/ttyS5";
    }
    else if(comboxTex == "COM7")
    {
        return "/dev/ttyS6";
    }
    else if(comboxTex == "COM8")
    {
        return "/dev/ttyS7";
    }
    else if(comboxTex == "COM9")
    {
        return "/dev/ttyS8";
    }
    else if(comboxTex == "COM1")
    {
        return "/dev/ttyS0";
    }
    else if(comboxTex == "COM10")
    {
        return "/dev/ttyS9";
    }
    else if(comboxTex == "COM1")
    {
        return "/dev/ttyS0";
    }
    else if(comboxTex == "COM11")
    {
        return "/dev/ttyS10";
    }
    else if(comboxTex == "COM12")
    {
        return "/dev/ttyS11";
    }
    else if(comboxTex == "USB-COM1")
    {
        return "/dev/ttyUSB0";
    }
    else if(comboxTex == "USB-COM2")
    {
        return "/dev/ttyUSB1";
    }
    else if(comboxTex == "USB-COM3")
    {
        return "/dev/ttyUSB2";
    }
    else if(comboxTex == "USB-COM4")
    {
        return "/dev/ttyUSB3";
    }
    else if(comboxTex == "XR-COM1")
    {
        return "/dev/ttyXR0";
    }
    else if(comboxTex == "XR-COM2")
    {
        return "/dev/ttyXR1";
    }
    else if(comboxTex == "XR-COM3")
    {
        return "/dev/ttyXR2";
    }
    else if(comboxTex == "XR-COM4")
    {
        return "/dev/ttyXR3";
    }
    return "";
}


QString Util::Uart_Revert(QString pTex)
{
    if(pTex == "/dev/ttyS0")
    {
        return "COM1";
    }
    else if(pTex == "/dev/ttyS1")
    {
        return "COM2";
    }
    else if(pTex == "/dev/ttyS2")
    {
        return "COM3";
    }
    else if(pTex == "/dev/ttyS3")
    {
        return "COM4";
    }
    else if(pTex == "/dev/ttyS4")
    {
        return "COM5";
    }
    else if(pTex == "/dev/ttyS5")
    {
        return "COM6";
    }
    else if(pTex == "/dev/ttyS6")
    {
        return "COM7";
    }
    else if(pTex == "/dev/ttyS7")
    {
        return "COM8";
    }
    else if(pTex == "/dev/ttyS8")
    {
        return "COM9";
    }
    else if(pTex == "/dev/ttyS9")
    {
        return "COM10";
    }
    else if(pTex == "/dev/ttyS10")
    {
        return "COM11";
    }
    else if(pTex == "/dev/ttyS11")
    {
        return "COM12";
    }
    else if(pTex == "/dev/ttyUSB0")
    {
        return "USB-COM1";
    }
    else if(pTex == "/dev/ttyUSB1")
    {
        return "USB-COM2";
    }
    else if(pTex == "/dev/ttyUSB2")
    {
        return "USB-COM3";
    }
    else if(pTex == "/dev/ttyUSB3")
    {
        return "USB-COM4";
    }
    else if(pTex == "/dev/ttyXR0")
    {
        return "XR-COM1";
    }
    else if(pTex == "/dev/ttyXR1")
    {
        return "XR-COM2";
    }
    else if(pTex == "/dev/ttyXR2")
    {
        return "XR-COM3";
    }
    else if(pTex == "/dev/ttyXR3")
    {
        return "XR-COM4";
    }

    return "";
}

