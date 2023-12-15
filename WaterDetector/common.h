#ifndef COMMON_H
#define COMMON_H

// custom
#include "cfactor.h"

// debug
#include <QDebug>

// json
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>

// file
#include <QDir>
#include <QFile>
#include <QFileInfo>

// 弹框
#include <QMessageBox>

// log
#include"QsLog/QsLog.h"
#include"QsLog/QsLogDest.h"
using namespace QsLogging;

// msgbox
#include "msgbox.h"

// database
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "string"
using namespace std;
//
// 宏定义
#define SYS_FILE "./system.json"
#define SYS_VERSION "V1.2.0"    // 2023-04-28

#define MODBUS_RTU "modbus-rtu"

#define FACTOR_PROPERTY "factor_property"
#define FACTOR_TAG_ID "factor_tag_id"
#define FACTOR_CODE "factor_code"
#define FACTOR_NAME "factor_name"
#define FACTOR_FUNC "factor_func"
#define FACTOR_ADDR "factor_addr"
#define BYTE_ORDER "byte_order"
#define DATA_TYPE "data_type"
#define TEMPLATE_PROTO "temp_proto"
#define TEMPLATE_NAME "temp_name"
#define DEV_NAME "dev_name"
#define DEV_ADDR "dev_addr"
#define DEV_TEMPLATE "dev_temp"
#define DEVICE "device"
#define TEMPLATE "template"
#define SERIAL "serial"
#define FACTORS "factors"

#define SERIAL_BAUDRATE "baudrate"
#define SERIAL_DATABIT "databit"
#define SERIAL_PARITY "parity"
#define SERIAL_NAME "serialname"
#define SERIAL_STOPBIT "stopbit"

#define UPLOAD "upload"
#define UPLOAD_MN "mn"
#define UPLOAD_IP "ip"
#define UPLOAD_PORT "port"
#define UPLOAD_PASSWORD "password"
#define UPLOAD_TIMEOUT "timeout"
#define UPLOAD_RETRY "retry"
#define UPLOAD_MIN_INTERNAL "min_internal"
#define UPLOAD_INTERNAL "upload_internal"
#define UPLOAD_HEARTBREAK "heartbreak"
#define UPLOAD_RT_UPLOAD "realtime_upload"

#define PLC_ADDR "PLC_Addr"

#define STARTTIME "StartTime"
#define INTERNALTIME "InternalTime"

// 数据类型
#define QT_INT16 "int16"
#define QT_UINT16 "uint16"
#define QT_INT32 "int32"
#define QT_UINT32 "uint32"
#define QT_FLOAT "float"

extern string g_StrQN;

#endif // COMMON_H

// 软件版本记录

/*
 * V1.0.0 发布第一版; 20220914;
 * V1.1.0 支持聚光-高指测量; 20220928;
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
