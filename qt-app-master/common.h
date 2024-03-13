#ifndef COMMON_H
#define COMMON_H

#include<unistd.h>

#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QTime>
#include <QFile>
#include <QProcess>
#include <QThread>
#include <QAbstractItemView>
#include <QScrollBar>
//#include "localkeyboard.h"
#include <QApplication>

#include "util.h"

#define CONF "./QDesktop.json"
#define QTAPP_VER "QDesktopV1.0.230307"
#define SCHEME "http"
//#define SERVER_IP "172.20.0.44"
//#define SERVER_IP "172.20.0.69"
#define SERVER_IP "172.20.0.218"
//#define SERVER_IP "172.20.0.238"
//#define SERVER_IP "172.26.191.58"
//#define SERVER_IP "localhost"
#define SERVER_PORT "8888"
#define REALTIME_FLUSH_PERIOD 5 // 实时数据刷新周期
#define USERNAME "admin"
#define PASSWORD "lcdcm"
#define TOKEN "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE5NTY1MjgwMDAsImlhdCI6MX0.0uQ36MB7Mt6i-tRzV9v1TnsftSoFpm6bJ_J8bQpUH4U"
#define PAGE_SIZE 10
#define DEVICE_CMD_INFO "/docu/cmdinfo.json"

//
#define TESHUZHI "teshuzhi"
#define CONF_TE_CODE "code"
#define CONF_TE_NAME "name"
#define CONF_TE_KEY "key"
#define CONF_TE_VALUE "value"

#define FACTORS "factors"
#define CONF_FACTOR_ALIAS "factor_alias"
#define CONF_IS_DEVICE_PROPERTY "isdeviceprop"
#define CONF_IS_ANALOG_PARAM "isanalogparam"
#define CONF_ANALOG_PARAM_AU1 "analogupper1"
#define CONF_ANALOG_PARAM_AD1 "analoglower1"
#define CONF_ANALOG_PARAM_AU2 "analogupper2"
#define CONF_ANALOG_PARAM_AD2 "analoglower2"

// HTTP api

#define DCM_CONF_FACTOR_DELETE "/dcm/set_qt_desktop_factors?device_factor_id="
#define DCM_CONF_FACTOR_EDIT "/dcm/set_qt_desktop_factors?device_factor_id="
#define DCM_DEVICE_FACTOR_DELETE "/dcm/device_factor?factor_id="
#define DCM_TESHUZHI_DELETE "/dcm/set_qt_desktop_teshuzhi?teshuzhi_fid="
#define DCM_DEVICE_DELETE "/dcm/device?device_id="
#define DCM_DEVICE "/dcm/device"
#define DCM_FACTOR "/dcm/factor"
#define DCM_CONF "/dcm/get_qt_desktop"
#define DCM_SYSTEM_CODE "/dcm/system_code"
#define DCM_DEVICE_FACTOR "/dcm/device_factor"
#define DCM_REALTIME_DATA "/dcm/realtime_data"
#define DCM_COONECT_STAT "/dcm/connect_stat"
#define DCM_VERSION "/dcm/version"
#define DCM_GET_IPADDR "/dcm/get_ipaddr"
#define DCM_SET_IPADDR "/dcm/set_ipaddr"
#define DCM_SERIAL_DEBUG "/dcm/serial_debug"
#define DCM_MNINFO "/dcm/mninfo"
#define DCM_MNINFO_DELETE "/dcm/mninfo?ip_addr_port="
#define DCM_MANUAL_MSG "/dcm/history/manual_send_msg?up_addr="
#define DCM_SET_TIME "/dcm/set_time?time="
#define DCM_SET_TESHUZHI "/dcm/set_qt_desktop_teshuzhi?teshuzhi_fid="
#define DCM_UP_MESSAGE "/dcm/history/up_message?up_addr="
#define DCM_HISTORY_RTD "/dcm/history/realtime_data?factor_code="
#define DCM_HISTORY "/dcm/history/"
#define DCM_SUPPORT_DEVICE "/dcm/suport_device"

#define CMDINFO "/docu/devicecmdinfo.json"
#define STATE_ON ":/images/checked.png"
#define STATE_OFF ":/images/unchecked.png"

extern bool m_LoginStatus;
extern QJsonObject g_Dcm_Factor;
extern QJsonArray g_Dcm_SystemCode;
extern QJsonObject g_Dcm_SupportDevice;
extern QJsonObject g_Dcm_Devices;
extern QJsonObject g_Dcm_Factors;
extern QJsonObject g_ConfObjDevParam;
extern QString g_Device_ID;
extern QString g_Device_Type;
extern bool g_IsAnalogDevOperated;




#endif // COMMON_H
