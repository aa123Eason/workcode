#include "hj212.h"
#include <QDebug>
#include "common.h"
using namespace std;
using std::chrono::system_clock;

vector<string> split(const string& str, const string& delim) {
    vector<string> res;
    if("" == str) return res;
    //先将要切割的字符串从string类型转换为char*类型
    char * strs = new char[str.length() + 1] ; //不要忘了
    strcpy(strs, str.c_str());

    char * d = new char[delim.length() + 1];
    strcpy(d, delim.c_str());

    char *p = strtok(strs, d);
    while(p) {
        string s = p; //分割得到的字符串转换为string类型
        res.push_back(s); //存入结果数组
        p = strtok(NULL, d);
    }

    return res;
}


std::string current_time()
{
    system_clock::time_point tp = system_clock::now();

    time_t raw_time = system_clock::to_time_t(tp);

    // tm*使用完后不用delete，因为tm*是由localtime创建的，并且每个线程中会有一个
    struct tm  *timeinfo  = std::localtime(&raw_time);

    char buf[24] = {0};
    // 标准c++中也可以使用"%F %X,"，但VC2017中不能这样用
    strftime(buf, 24, "%Y%m%d%H%M%S", timeinfo);

    // tm只能到秒，毫秒需要另外获取
    // std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());

    // std::string milliseconds_str =  std::to_string(ms.count() % 1000);

    //if (milliseconds_str.length() < 3) {
    //    milliseconds_str = std::string(3 - milliseconds_str.length(), '0') + milliseconds_str;
    //}
    std::string milliseconds_str = "000";

    return std::string(buf+ milliseconds_str);
}

std::string current_datetime()
{
    system_clock::time_point tp = system_clock::now();

    time_t raw_time = system_clock::to_time_t(tp);

    // tm*使用完后不用delete，因为tm*是由localtime创建的，并且每个线程中会有一个
    struct tm  *timeinfo  = std::localtime(&raw_time);
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;

    char buf[24] = {0};
    // 标准c++中也可以使用"%F %X,"，但VC2017中不能这样用
    strftime(buf, 24, "%Y%m%d%H%M%S", timeinfo);

    std::string milliseconds_str = "000";

    return std::string(buf+ milliseconds_str);
}

std::string current_formattime()
{
    system_clock::time_point tp = system_clock::now();

    time_t raw_time = system_clock::to_time_t(tp);

    // tm*使用完后不用delete，因为tm*是由localtime创建的，并且每个线程中会有一个
    struct tm  *timeinfo  = std::localtime(&raw_time);

    char buf[24] = {0};
    // 标准c++中也可以使用"%F %X,"，但VC2017中不能这样用
    strftime(buf, 24, "%Y-%m-%d %H:%M:%S", timeinfo);

    return std::string(buf);
}

std::string current_minutetime()
{
    system_clock::time_point tp = system_clock::now();

    time_t raw_time = system_clock::to_time_t(tp);

    // tm*使用完后不用delete，因为tm*是由localtime创建的，并且每个线程中会有一个
    struct tm  *timeinfo  = std::localtime(&raw_time);
    timeinfo->tm_sec = 0;

    char buf[24] = {0};
    // 标准c++中也可以使用"%F %X,"，但VC2017中不能这样用
    strftime(buf, 24, "%Y-%m-%d %H:%M:%S", timeinfo);

    return std::string(buf);
}

string  hj212_data_encode(HJ212_PARAM_SET *hj212_params)
{
    string data = "";
    int flag_first = 0;

    HJ212_DATA_PARAM *data_params = hj212_params->hj212_data_params;

    if(data_params->DataTime != "")
    {
        if(flag_first == 0)
        {
           flag_first = 1;
        }
        else
        {
            data += ";";
        }
        data += HJ212_FIELD_NAME_DATA_TIME + data_params->DataTime;
    }

    if(data_params->Rtd != "")
    {
        if(flag_first == 0)
        {
           flag_first = 1;
        }
        else
        {
            data += ";";
        }
        data += data_params->Rtd;
    }

    return data;
}

unsigned int CRC16_Cal( unsigned char *puchMsg, unsigned int usDataLen )
{
    unsigned int i,j,crc_reg,check;
    crc_reg = 0xFFFF;
    for(i=0;i<usDataLen;i++)
    {
        crc_reg = (crc_reg>>8) ^ puchMsg[i];
        for(j=0;j<8;j++)
        {
            check = crc_reg & 0x0001;
            crc_reg >>= 1;
            if(check==0x0001)
            {
                crc_reg ^= 0xA001;
            }
         }
     }
     return crc_reg;
}

std::string dec2hex(int i, int width)
{
    std::stringstream ioss; //定义字符串流
    std::string s_temp; //存放转化后字符
    ioss << std::hex << i; //以十六制形式输出
    ioss >> s_temp;
    std::string s(width - s_temp.size(), '0'); //补0
    s += s_temp; //合并
    return s;
}

int crc_check(string frame)
{
    std::size_t found;
    if(std::string::npos == (found = frame.find(HJ212_FRAME_CP_START)))
    {
        return 1;
    }

    std::size_t found1;
    found1 = frame.find(HJ212_FRAME_CP_END, found + 5);
    string crc = frame.substr(found1 + 2, 4);

    int data_len = frame.length() - HJ212_BYTES_FRAME_BAOTOU - HJ212_BYTES_FRAME_LEN - HJ212_BYTES_FRAME_CRC - HJ212_BYTES_FRAME_BAOWEI;
    string sp_data = frame.substr(HJ212_BYTES_FRAME_BAOTOU + HJ212_BYTES_FRAME_LEN, data_len);
    uint16_t crc_val = CRC16_Cal((unsigned char*)sp_data.data(), sp_data.length());

    int crc_hex_val = 0;
    sscanf(crc.data(), "%x", &crc_hex_val);
    qDebug() << "crc_hex_val:" << crc_hex_val;


    if(crc_hex_val != crc_val)
    {
        qDebug() << "error, get crc:" << QString::fromStdString(crc) << "right crc:" << crc_val;
        return 1;
    }

    return 0;
}

string  hj212_data_segment_encode(HJ212_PARAM_SET *hj212_params)
{
    string data_segment;
    data_segment = HJ212_FRAME_QN +  hj212_params->QN + ";";
    data_segment += HJ212_FRAME_ST + hj212_params->ST + ";";
    data_segment += HJ212_FRAME_CN + hj212_params->CN + ";";
    data_segment += HJ212_FRAME_PW + hj212_params->PW + ";";
    data_segment += HJ212_FRAME_MN + hj212_params->MN + ";";
    data_segment += HJ212_FRAME_FLAG + hj212_params->Flag + ";";

    if(hj212_params->Flag != "")
    {
        if(hj212_params->PNUM != "")
        {
            data_segment += HJ212_FRAME_PNUM + hj212_params->PNUM + ";";
        }

        if(hj212_params->PNO != "")
        {
            data_segment += HJ212_FRAME_PNO + hj212_params->PNO + ";";
        }
    }

    string data;
    data = hj212_data_encode(hj212_params);
    data_segment += HJ212_FRAME_CP_START + data + HJ212_FRAME_CP_END;

    return data_segment;
}

string  hj212_frame_assemble(HJ212_PARAM_SET *hj212_params)
{
    string data_segment = hj212_data_segment_encode(hj212_params);
    uint16_t crc_val = CRC16_Cal((unsigned char*)data_segment.data(), data_segment.length());
    string crc = dec2hex(crc_val, 4);
    string frame;
    string data_len_str;
    if(data_segment.length() < 9)
    {
        data_len_str = "000" + to_string(data_segment.length());
    }
    else if((data_segment.length()>9) && (data_segment.length()< 99))
    {
        data_len_str = "00" + to_string(data_segment.length());
    }
    else if((data_segment.length()>99) && (data_segment.length()< 999))
    {
        data_len_str = "0" + to_string(data_segment.length());
    }
    else
    {
       data_len_str = to_string(data_segment.length());
    }

    frame += HJ212_FRAME_HEADER + data_len_str + data_segment + crc +HJ212_FRAME_TAIL;
    return frame;
}

HJ212_PARAM_SET hj212_set_params(string ST, string CN, string Flag, HJ212_DATA_PARAM *hj212_data_param, string pw, string mn)
{
    HJ212_PARAM_SET hj212_param_set;
    hj212_param_set.PW = pw;
    hj212_param_set.MN = mn;
    hj212_param_set.ST = ST;
    hj212_param_set.CN = CN;
    if(g_StrQN == "") hj212_param_set.QN = current_time();
    else hj212_param_set.QN = g_StrQN;

    hj212_param_set.Flag = Flag;
    hj212_param_set.hj212_data_params = hj212_data_param;

    return hj212_param_set;
}

string hj212_frame_parse(string frame, MENJIN_SET_PARAMS* menjin_set_paramsIn, string pw, string mn)
{
    string::size_type found;
    if(std::string::npos != (found = frame.find(HJ212_FRAME_QN)))
    {
        unsigned pos;
        pos = frame.find(";", found);
        g_StrQN = frame.substr(found + 3, pos - found - 3);
    }

    if(std::string::npos != (found = frame.find(HJ212_FRAME_PW)))
    {
        unsigned pos;
        pos = frame.find(";", found);
        string pw_frame = frame.substr(found + 3, pos - found - 3);
        if(0 != strcmp(pw_frame.data(), pw.data()))
        {
            HJ212_DATA_PARAM hj212_data_param;
            hj212_data_param.QnRtn = HJ212_QNRTN_ERR_PW;

            HJ212_PARAM_SET hj212_param;
            hj212_param = hj212_set_params(HJ21_DEF_ACK_ST, HJ212_CN_GET_QNRTN, HJ212_FLAG_ACK_NO, &hj212_data_param, pw, mn);
            string rep = hj212_frame_assemble(&hj212_param);
            menjin_set_paramsIn->type = -1;
            return rep;
        }
    }
    else
    {
        HJ212_DATA_PARAM hj212_data_param;
        hj212_data_param.QnRtn = HJ212_QNRTN_ERR_UNKNOWN;

        HJ212_PARAM_SET hj212_param;
        hj212_param = hj212_set_params(HJ21_DEF_ACK_ST, HJ212_CN_GET_QNRTN, HJ212_FLAG_ACK_NO, &hj212_data_param, pw, mn);
        string rep = hj212_frame_assemble(&hj212_param);
        menjin_set_paramsIn->type = -1;
        return rep;
    }

    if(std::string::npos != (found = frame.find(HJ212_FRAME_ST)))
    {
        unsigned pos;
        pos = frame.find(";", found);
        string st = frame.substr(found + 3, pos - found - 3);
        if(0 != strcmp(st.data(), HJ212_DEF_VAL_ST))
        {
            HJ212_DATA_PARAM hj212_data_param;
            hj212_data_param.QnRtn = HJ212_QNRTN_ERR_ST;

            HJ212_PARAM_SET hj212_param;
            hj212_param = hj212_set_params(HJ21_DEF_ACK_ST, HJ212_CN_GET_QNRTN, HJ212_FLAG_ACK_NO, &hj212_data_param, pw, mn);
            string rep = hj212_frame_assemble(&hj212_param);
            menjin_set_paramsIn->type = -1;
            return rep;
        }
    }
    else
    {
        HJ212_DATA_PARAM hj212_data_param;
        hj212_data_param.QnRtn = HJ212_QNRTN_ERR_UNKNOWN;
        HJ212_PARAM_SET hj212_param;
        hj212_param = hj212_set_params(HJ21_DEF_ACK_ST, HJ212_CN_GET_QNRTN, HJ212_FLAG_ACK_NO, &hj212_data_param, pw, mn);
        string rep = hj212_frame_assemble(&hj212_param);
        menjin_set_paramsIn->type = -1;
        return rep;
    }

    if(std::string::npos != (found = frame.find(HJ212_FRAME_MN)))
    {
        unsigned pos;
        pos = frame.find(";", found);
        string mn_frame = frame.substr(found + 3, pos - found - 3);
        if(0 != strcmp(mn_frame.data(), mn.data()))
        {
            HJ212_DATA_PARAM hj212_data_param;
            hj212_data_param.QnRtn = HJ212_QNRTN_ERR_MN;

            HJ212_PARAM_SET hj212_param;
            hj212_param = hj212_set_params(HJ21_DEF_ACK_ST, HJ212_CN_GET_QNRTN, HJ212_FLAG_ACK_NO, &hj212_data_param, pw, mn);
            string rep = hj212_frame_assemble(&hj212_param);
            menjin_set_paramsIn->type = -1;
            return rep;
        }
    }
    else
    {
        HJ212_DATA_PARAM hj212_data_param;
        hj212_data_param.QnRtn = HJ212_QNRTN_ERR_UNKNOWN;
        HJ212_PARAM_SET hj212_param;
        hj212_param = hj212_set_params(HJ21_DEF_ACK_ST, HJ212_CN_GET_QNRTN, HJ212_FLAG_ACK_NO, &hj212_data_param, pw, mn);
        string rep = hj212_frame_assemble(&hj212_param);
        menjin_set_paramsIn->type = -1;
        return rep;
    }

    if(std::string::npos != (found = frame.find(HJ212_FRAME_FLAG)))
    {
        unsigned pos;
        pos = frame.find(";", found);
        string flag = frame.substr(found + 5, pos - found - 5);
        if(0 != strcmp(flag.data(), HJ212_FLAG_ACK_EN))
        {
            HJ212_DATA_PARAM hj212_data_param;
            hj212_data_param.QnRtn = HJ212_QNRTN_ERR_FLAG;

            HJ212_PARAM_SET hj212_param;
            hj212_param = hj212_set_params(HJ21_DEF_ACK_ST, HJ212_CN_GET_QNRTN, HJ212_FLAG_ACK_NO, &hj212_data_param, pw, mn);
            string rep = hj212_frame_assemble(&hj212_param);
            menjin_set_paramsIn->type = -1;
            return rep;
        }
    }
    else
    {
        HJ212_DATA_PARAM hj212_data_param;
        hj212_data_param.QnRtn = HJ212_QNRTN_ERR_UNKNOWN;
        HJ212_PARAM_SET hj212_param;
        hj212_param = hj212_set_params(HJ21_DEF_ACK_ST, HJ212_CN_GET_QNRTN, HJ212_FLAG_ACK_NO, &hj212_data_param, pw, mn);
        string rep = hj212_frame_assemble(&hj212_param);
        menjin_set_paramsIn->type = -1;
        return rep;
    }

    if(std::string::npos != (found = frame.find(HJ212_FRAME_CN)))
    {
        unsigned pos;
        pos = frame.find(";", found);
        string cn = frame.substr(found + 3, pos - found - 3);
        if(0 == strcmp(cn.data(), HJ212_CN_REMOTE_OPEN_MJ))
        {
            menjin_set_paramsIn->type = 1;
        }
        else if(0 == strcmp(cn.data(), HJ212_CN_MJ_EMPOWER_SET))
        {
            menjin_set_paramsIn->type = 0;
        }
        else
        {
            HJ212_DATA_PARAM hj212_data_param;
            hj212_data_param.QnRtn = HJ212_QNRTN_ERR_CN;

            HJ212_PARAM_SET hj212_param;
            hj212_param = hj212_set_params(HJ21_DEF_ACK_ST, HJ212_CN_GET_QNRTN, HJ212_FLAG_ACK_NO, &hj212_data_param, pw, mn);
            string rep = hj212_frame_assemble(&hj212_param);
            menjin_set_paramsIn->type = -1;
            return rep;
        }
    }
    else
    {
        HJ212_DATA_PARAM hj212_data_param;
        hj212_data_param.QnRtn = HJ212_QNRTN_ERR_UNKNOWN;
        HJ212_PARAM_SET hj212_param;
        hj212_param = hj212_set_params(HJ21_DEF_ACK_ST, HJ212_CN_GET_QNRTN, HJ212_FLAG_ACK_NO, &hj212_data_param, pw, mn);
        string rep = hj212_frame_assemble(&hj212_param);
        menjin_set_paramsIn->type = -1;
        return rep;
    }

    if(crc_check(frame))
    {
        HJ212_DATA_PARAM hj212_data_param;
        hj212_data_param.QnRtn = HJ212_QNRTN_ERR_CRC;

        HJ212_PARAM_SET hj212_param;
        hj212_param = hj212_set_params(HJ21_DEF_ACK_ST, HJ212_CN_GET_QNRTN, HJ212_FLAG_ACK_NO, &hj212_data_param, pw, mn);
        string rep = hj212_frame_assemble(&hj212_param);
        menjin_set_paramsIn->type = -1;
        return rep;
    }

    if(std::string::npos != (found = frame.find(HJ212_FACTOR_MJ_CODE)))
    {
        unsigned pos;
        pos = frame.find(";", found);
        menjin_set_paramsIn->menji_code = frame.substr(found + 12, pos - found - 12);
    }
    else
    {
        HJ212_DATA_PARAM hj212_data_param;
        hj212_data_param.QnRtn = HJ212_QNRTN_ERR_UNKNOWN;
        HJ212_PARAM_SET hj212_param;
        hj212_param = hj212_set_params(HJ21_DEF_ACK_ST, HJ212_CN_GET_QNRTN, HJ212_FLAG_ACK_NO, &hj212_data_param, pw, mn);
        string rep = hj212_frame_assemble(&hj212_param);
        menjin_set_paramsIn->type = -1;
        return rep;
    }

    if(std::string::npos != (found = frame.find(HJ212_FACTOR_MJ_USER_ID)))
    {
        unsigned pos;
        pos = frame.find(";", found);
        menjin_set_paramsIn->user_id = frame.substr(found + 12, pos - found - 12);
    }
    else
    {
        HJ212_DATA_PARAM hj212_data_param;
        hj212_data_param.QnRtn = HJ212_QNRTN_ERR_UNKNOWN;
        HJ212_PARAM_SET hj212_param;
        hj212_param = hj212_set_params(HJ21_DEF_ACK_ST, HJ212_CN_GET_QNRTN, HJ212_FLAG_ACK_NO, &hj212_data_param, pw, mn);
        string rep = hj212_frame_assemble(&hj212_param);
        menjin_set_paramsIn->type = -1;
        return rep;
    }

    if(menjin_set_paramsIn->type == 0)
    {
        string::size_type found1;
        string::size_type found2;

        if((std::string::npos != (found = frame.find(HJ212_FACTOR_MJ_IMAGE_ADDR))) && (std::string::npos != (found1 = frame.find(HJ212_FACTOR_MJ_USERNAME))) && (std::string::npos != (found2 = frame.find(HJ212_FIELD_NAME_SFP))))
        {
            unsigned pos;
            pos = frame.find(";", found);
            menjin_set_paramsIn->image_addr = frame.substr(found + 12, pos - found - 12);

            pos = frame.find(";", found1);
            menjin_set_paramsIn->username = frame.substr(found1 + 12, pos - found1 - 12);

            menjin_set_paramsIn->SFP = atoi(frame.substr(found2 + 4, 1).c_str());
        }
        else
        {
            HJ212_DATA_PARAM hj212_data_param;
            hj212_data_param.QnRtn = HJ212_QNRTN_ERR_UNKNOWN;
            HJ212_PARAM_SET hj212_param;
            hj212_param = hj212_set_params(HJ21_DEF_ACK_ST, HJ212_CN_GET_QNRTN, HJ212_FLAG_ACK_NO, &hj212_data_param, pw, mn);
            string rep = hj212_frame_assemble(&hj212_param);
            menjin_set_paramsIn->type = -1;
            return rep;
        }
    }

    HJ212_DATA_PARAM hj212_data_param;
    hj212_data_param.QnRtn = HJ212_QNRTN_EXCUTE;

    HJ212_PARAM_SET hj212_param;
    hj212_param = hj212_set_params(HJ21_DEF_ACK_ST, HJ212_CN_GET_QNRTN, HJ212_FLAG_ACK_NO, &hj212_data_param, pw, mn);
    string rep = hj212_frame_assemble(&hj212_param);

    return rep;
}



