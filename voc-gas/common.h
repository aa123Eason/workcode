#ifndef COMMON_H
#define COMMON_H

#include <QMessageBox>

// file
#include <QDir>
#include <QFile>
#include <QFileInfo>

// json
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>

#include "string"
using namespace std;
extern string g_StrQN;

// log
#include"QsLog/QsLog.h"
#include"QsLog/QsLogDest.h"
using namespace QsLogging;

#define FACTORS_SETTING_FILE "D:/voc-factors.json"
#define COMM_SETTING_FILE "D:/voc-comm.json"
#define SYSTEM_SETTING_FILE "D:/voc-system.json"
#define FAN_SETTING_FILE "D:/voc-fan.json"
#define USERS_SETTING_FILE "D:/voc-users.json"
#define LOG_PATH "D:/voc-log/"

#define FACTORS "factors"
#define UNIT "Unit"
#define CHAN "Chan"
#define RANGEUPPER "RangeUpper"
#define RANGELOWER "RangeLower"
#define ALARMUPPER "AlarmUpper"
#define DISPLAY "Display"
#define UPLOAD "Upload"
#define USED "Used"
#define INDEX "Index"
#define VALUE "Value"

#define PORTSUP "ports-up"
#define PORTSDOWN "ports-down"
#define DEVICE "device"
#define PORTNAME "name"
#define BAUD "baudrate"
#define PARITY "parity"
#define DATABIT "databit"
#define STOPBIT "stopbit"
#define DEVICETYPE "type"
#define MODBUS "Modbus"
#define PLC "PLC"
#define HJ212 "HJ212"
#define HJ212_2 "HJ212-2"

#define USERS "users"
#define USERTYPE "type"
#define USERPASSWD "password"
#define USESET_QINGKONG "clear"
#define USESET_CANSHU "param"
#define USESET_FANCHUI "fanchui"
#define USESET_SHIJIAN "time"
#define USESET_LISHI "history"
#define USESET_QUXIAN "curve"
#define USESET_FIDA "fid-a"
#define USESET_FIDB "fid-b"
#define USESET_WENDU "temprature"
#define USESET_ZHUANGTAI "state"

#define SYS_SET1 "sys1"
#define SYS_SET2 "sys2"
#define SYS_SET3 "sys3"
#define SYS_SET4 "sys4"
#define SYS_SET5 "sys5"
#define SYS_SET6 "sys6"
#define SYS_SET7 "sys7"
#define SYS_SET8 "sys8"
#define SYS_CHECK "checkBox"
#define SYS_RADIOBUTTON1 "radioButton1"
#define SYS_RADIOBUTTON2 "radioButton2"

#define FAN_SET "fan_set"
#define FAN_LAST "fan_last"



#endif // COMMON_H
