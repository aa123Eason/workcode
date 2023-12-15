#ifndef HJ212_H
#define HJ212_H

#include <iostream>
#include <chrono>
#include <string>
#include <memory>
#include <ctime>
#include <sstream>
#include <cstring>
#include <vector>

using namespace std;
using std::chrono::system_clock;

//帧格式
#define HJ212_FRAME_HEADER				"##"
#define HJ212_FRAME_QN					"QN="
#define HJ212_FRAME_ST					"ST="
#define HJ212_FRAME_CN					"CN="
#define HJ212_FRAME_PW					"PW="
#define HJ212_FRAME_MN					"MN="
#define HJ212_FRAME_FLAG				"Flag="
#define HJ212_FRAME_PNUM				"PNUM="
#define HJ212_FRAME_PNO					"PNO="
#define HJ212_FRAME_CP_START			"CP=&&"
#define HJ212_FRAME_CP_END				"&&"
#define HJ212_FRAME_TAIL				"\r\n"

#define HJ212_BYTES_FRAME_BAOTOU		2
#define HJ212_BYTES_FRAME_LEN			4
#define HJ212_BYTES_FRAME_CRC			4
#define HJ212_BYTES_FRAME_BAOWEI		2
#define HJ212_BYTES_FRAME_QN			20
#define HJ212_BYTES_FRAME_ST			5
#define HJ212_BYTES_FRAME_CN			7
#define HJ212_BYTES_FRAME_PW			9
#define HJ212_BYTES_FRAME_MN			27
#define HJ212_BYTES_QN					17
#define HJ212_BYTES_ST					2
#define HJ212_BYTES_CN					4
#define HJ212_BYTES_PW					6
#define HJ212_BYTES_MN					24

//拆分包及应答标志
#define HJ212_FLAG_ACK_NO				"4"
#define HJ212_FLAG_PACKET_DIV_EN		"6"
#define HJ212_FLAG_ACK_EN				"5"


//相关配置信息
//#define HJ212_ST_SYS_COMM				"91"
//#define HJ212_BYTES_TIME_CALI_REQ		150
//#define HJ212_BYTES_DATA_RTD			230
//#define HJ212_BYTES_DATA_MIN			460
//#define HJ212_REPORT_REPLY_EN			RT_FALSE


//字段名
#define HJ212_FIELD_NAME_SYSTIME		"SystemTime="
#define HJ212_FIELD_NAME_QNRTN			"QnRtn="
#define HJ212_FIELD_NAME_EXERTN			"ExeRtn="
#define HJ212_FIELD_NAME_RTDINTERVAL	"RtdInterval="
#define HJ212_FIELD_NAME_MININTERVAL	"MinInterval="
#define HJ212_FIELD_NAME_RESTARTTIME	"RestartTime="
#define HJ212_FIELD_NAME_POLID          "PolId="
#define HJ212_FIELD_NAME_SAMPLE_TIME    "-SampleTime="
#define HJ212_FIELD_NAME_DATA_RTD		"-Rtd="
#define HJ212_FIELD_NAME_DATA_MIN		"-Min="
#define HJ212_FIELD_NAME_DATA_AVG		"-Avg="
#define HJ212_FIELD_NAME_DATA_MAX		"-Max="
#define HJ212_FIELD_NAME_DATA_ZS_RTD    "-ZsMin="
#define HJ212_FIELD_NAME_DATA_ZS_MIN	"-ZsMin="
#define HJ212_FIELD_NAME_DATA_ZS_AVG	"-ZsAvg="
#define HJ212_FIELD_NAME_DATA_ZS_MAX	"-ZsMax="
#define HJ212_FIELD_NAME_DATA_FLAG		"-Flag="
#define HJ212_FIELD_NAME_DATA_EFLAG		"-EFlag="
#define HJ212_FIELD_NAME_DATA_COU		"-Cou="
//#define HJ212_FIELD_NAME_RUN_STA		"SB%1-RS=;"
//#define HJ212_FIELD_NAME_RUN_TIME		"SB%1-RT=;"
#define HJ212_FIELD_NAME_BEGIN_TIME		"BeginTime="
#define HJ212_FIELD_NAME_END_TIME		"EndTime="
#define HJ212_FIELD_NAME_DATA_TIME		"DataTime="
#define HJ212_FIELD_NAME_NEW_PW			"NewPW="
#define HJ212_FIELD_NAME_OVER_TIME		"OverTime="
#define HJ212_FIELD_NAME_RECOUNT		"ReCount="
#define HJ212_FIELD_NAME_CSTART_TIME	"CstartTime="
#define HJ212_FIELD_NAME_C_TIME		    "Ctime="
#define HJ212_FIELD_NAME_S_TIME		    "Stime="
#define HJ212_FIELD_NAME_INFO_ID		"InfoId="
#define HJ212_FIELD_NAME_INFO			"-InfoId="
#define HJ212_FIELD_NAME_SN		        "-SN="


#define HJ212_FIELD_NAME_DT		        "DT="
#define HJ212_FIELD_NAME_SFP            "SFP="


//exe rtn
#define HJ212_EXERTN_OK					"1"
#define HJ212_EXERTN_ERR_NO_REASON		"2"
#define HJ212_EXERTN_ERR_CMD			"3"
#define HJ212_EXERTN_ERR_COMM			"4"
#define HJ212_EXERTN_ERR_BUSY			"5"
#define HJ212_EXERTN_ERR_SYSTEM			"6"
#define HJ212_EXERTN_ERR_NO_DATA		"100"

//qn rtn
#define HJ212_QNRTN_EXCUTE				"1"
#define HJ212_QNRTN_REJECTED			"2"
#define HJ212_QNRTN_ERR_PW				"3"
#define HJ212_QNRTN_ERR_MN				"4"
#define HJ212_QNRTN_ERR_ST				"5"
#define HJ212_QNRTN_ERR_FLAG			"6"
#define HJ212_QNRTN_ERR_QN				"7"
#define HJ212_QNRTN_ERR_CN				"8"
#define HJ212_QNRTN_ERR_CRC				"9"
#define HJ212_QNRTN_ERR_UNKNOWN			"100"

//数据标记
#define HJ212_DATA_FLAG_RUN				'N'
#define HJ212_DATA_FLAG_STOP			'F'
#define HJ212_DATA_FLAG_MAINTAIN		'M'
#define HJ212_DATA_FLAG_MANUAL			'S'
#define HJ212_DATA_FLAG_ERR				'D'
#define HJ212_DATA_FLAG_CALIBRATE		'C'
#define HJ212_DATA_FLAG_LIMIT			'T'
#define HJ212_DATA_FLAG_COMM			'B'

//命令编码
#define HJ212_CN_SET_OT_RECOUNT			"1000"
#define HJ212_CN_GET_SYS_TIME			"1011"
#define HJ212_CN_SET_SYS_TIME			"1012"
#define HJ212_CN_REQ_SYS_TIME			"1013"
#define HJ212_CN_GET_RTD_INTERVAL		"1061"
#define HJ212_CN_SET_RTD_INTERVAL		"1062"
#define HJ212_CN_GET_MIN_INTERVAL		"1063"
#define HJ212_CN_SET_MIN_INTERVAL		"1064"
#define HJ212_CN_SET_NEW_PW				"1072"
#define HJ212_CN_RTD_DATA				"2011"
#define HJ212_CN_STOP_RTD_DATA			"2012"
#define HJ212_CN_RS_DATA				"2021"
#define HJ212_CN_STOP_RS_DATA			"2022"
#define HJ212_CN_DAY_DATA				"2031"
#define HJ212_CN_DAY_RS_DATA			"2041"
#define HJ212_CN_MIN_DATA				"2051"
#define HJ212_CN_HOUR_DATA				"2061"
#define HJ212_CN_RESTART_TIME			"2081"
#define HJ212_CN_UPDATE                 "3020"   //上传命令
#define HJ212_CN_GET_QNRTN				"9011"   //请求应答命令编码
#define HJ212_CN_GET_EXERTN				"9012"   //执行结果应答
#define HJ212_CN_GET_INFORM_RTN			"9013"
#define HJ212_CN_GET_DATA_RTN			"9014"

#define HJ212_CN_REMOTE_OPEN_MJ         "4022"  //远程开启门禁
#define HJ212_CN_MJ_EMPOWER_SET         "4024"  //门禁授权下发

//监测因子编码
#define HJ212_FACTOR_MJ_CODE				"i3310A-Info="
#define HJ212_FACTOR_MJ_OPEN_TYPE           "i3310B-Info="
#define HJ212_FACTOR_MJ_USER_ID			    "i3310D-Info="
#define HJ212_FACTOR_MJ_OPEN_DATE			"i3310E-Info="
#define HJ212_FACTOR_MJ_DOOR_STATUS			"i3310F-Info="
#define HJ212_FACTOR_MJ_IMAGE_ADDR			"i3310I-Info="
#define HJ212_FACTOR_MJ_USERNAME			"i3310J-Info="

//参数默认值
#define HJ21_DEF_ACK_ST                 "91"
#define HJ212_DEF_VAL_ST				"21"
#define HJ212_DEF_VAL_PW				"123456"
#define HJ212_DEF_VAL_MN				"00112233445566778899AABB"

typedef struct hj212_data_param{
    string SystemTime;
    string QnRtn;
    string ExeRtn;
    string RtdInterval;
    string MinInterval;
    string RestartTime;
    string PolId;
    string SampleTime;
    string Rtd;
    string Min;
    string Avg;
    string Max;
    string ZsRtd;
    string ZsMin;
    string ZsAvg;
    string ZsMax;
    string Flag;
    string EFlag;
    string Cou;
//    string RS;
//    string RT;
//    string Data;
//    string DayData;
//    string NightData;
    string BeginTime;
    string EndTime;
    string DataTime;
    string NewPW;
    string OverTime;
    string ReCount;
//    string VaseNo;
    string CstartTime;
    string Ctime;
    string Stime;
    string InfoId;
    string SN;
    string DT;
    string i3310A_Info;
    string i3310B_Info;
    string i3310D_Info;
    string i3310E_Info;
    string i3310F_Info;
}HJ212_DATA_PARAM;

//参数集合
typedef struct hj212_param_set
{
    string 			QN;     //请求编码
    string 			ST;     //系统编码
    string 			CN;     //命令编码
    string			PW;     //访问密码
    string			MN;     //设备唯一标识
    string          Flag;   //拆分包及应答标志
    string          PNUM;   //总包数
    string          PNO;    //包号
    string          CP;   //指令参数
    HJ212_DATA_PARAM *hj212_data_params;
} HJ212_PARAM_SET;

typedef struct Menjin_set_param{
    int type;  // 0:门禁授权下发 1:远程开启门禁 -1: error
    string menji_code;  //门禁编码
    string user_id;     //系统中用户唯一Id
    string username;    //人员姓名
    string image_addr;  //人员图像地址
    int SFP;            //人脸识别是否启用，0 停用，1 启用
}MENJIN_SET_PARAMS;

extern string current_time();
extern string current_datetime();
extern string current_minutetime();
extern string current_formattime();
extern string  hj212_frame_assemble(HJ212_PARAM_SET *hj212_params);
extern HJ212_PARAM_SET hj212_set_params(string ST, string CN, string Flag, HJ212_DATA_PARAM *hj212_data_param, string pw, string mn);
extern string hj212_frame_parse(string frame, MENJIN_SET_PARAMS* menjin_set_paramsIn, string pw, string mn);

#endif // HJ212_H
