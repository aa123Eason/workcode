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

QMap<QString,QStringList> Util::Uart_devicetype()
{
    QMap<QString,QStringList> map;

    map["9bo-liuliangji"]<<"九波流量计"<<"累计流量"<<"瞬时流量";
    map["air-quality"]<<"航天新气象-空气质量传感器"<<"PM2.5"<<"PM10"<<"SO2"<<"CO"<<"NO2"<<"O3"<<"TEMP"<<"HUMI";
    map["an-dan"]<<"氨氮"<<"氨氮";
    map["analog"]<<"模拟量"<<"analog_max>模拟量量程上限"<<"analog_min>模拟量量程下限"<<"upper_limit>输入范围上限"<<"lower_limit>输入范围下限";
    map["analog-sichuan"]<<"模拟量(四川版本)"<<"analog_max>模拟量量程上限"<<"analog_min>模拟量量程下限"<<"upper_limit>输入范围上限"<<"lower_limit>输入范围下限";
    map["co2-sensor"]<<"航天新气象-co2"<<"CO2";
    map["conductivity-v1.4"]<<"V1.4_无型号版本电导率"<<"主测量值(电导率)"<<"次测量值(按实际情况)";
    map["derun-803m"]<<"德润DR-803M-MOBUSRTU寄存器-水质自动采样仪"<<"工作模式"<<"当前瓶位置"<<"平行样数"<<"留样瓶数"<<"每瓶留样次数"<<"每次留样量"<<"当前进程"<<"当前瓶已留次数"<<"采样器状态";
    map["derun-803n-nanjing"]<<"DR-803N-南京MOBUSRTU寄存器-水质自动采样仪"<<"设备状态"<<"当前瓶位置"<<"空瓶数"<<"留样时间"<<"是否有固定剂"<<"留样量"<<"远程留样方式"<<"加药类型"<<"加药比例"<<"样品编号"<<"门禁密码"<<"留样结果"<<"留样瓶状态"<<"报警状态"<<"工作模式"<<"当前留样瓶数"<<"exten_cn=3020";
    map["dx-rtu-1"]<<"DX-RTU-1"<<"水位"<<"流速"<<"瞬时流量"<<"累积流量";
    map["guizhou-dtgk"]<<"贵州动态管控协议"<<"水样浓度"<<"水样吸光度"<<"水样测量时间"<<"零点校准浓度"<<"零点吸光度"<<"零点校准时间"<<"量程校准浓度"<<"量程吸光度"<<"量程校准时间"<<"量程"<<"斜率"<<"截距"<<"修正斜率"<<"修正截距"<<"消解时间"<<"消解温度"<<"核查设置浓度"<<"核查吸光度"<<"核查结果浓度"<<"核查偏差"<<"核查时间"<<"检测时间"<<"工作状态"<<"故障状态"<<"数据标志"<<"报警状态";
    map["guohong-bluesky"]<<"国弘多参数"<<"pH"<<"电导率Cond（μS/cm）"<<"溶解氧Do（mg/L）"<<"浊度Turb（NTU）"<<"温度Temp（℃）"<<"余氯ClO-（mg/L）"<<"氨氮NH4-N（mg/L）"<<"化学需氧量COD（mg/L）"<<"氧化还原电位ORP（mV）"<<"叶绿素（μg/L）"<<"蓝绿藻（Kcells/mL）"<<"氟化物（mg/L）"<<"透明度（cm）"<<"氯化物（mg/L）"<<"水位（m）"<<"硝酸盐（mg/L）"<<"pH(历史)"<<"电导率(历史)"<<"溶解氧(历史)"<<"浊度(历史)"<<"温度(历史)"<<"历史时间戳";
    map["hj212-serial"]<<"hj212串口";
    map["l-mag"]<<"l-mag流量计"<<"瞬时流量"<<"累计流量";
    map["lc-frms-1000"]<<"固定辐射监测传感器"<<"辐射剂量";
    map["lc-modbus"]<<"modbus通用驱动"<<"start>起始寄存器地址"<<"quantity>寄存器数量"<<"functionCode>功能码:1,2,3,4"<<"vtype>值类型:int16,long,long-inverse,float32,float3-4321,float32-inverse,float64,float64-inverse,coils线圈";
    map["lc-water-a"]<<"蓝创A款"<<"是否有效测量值"<<"测量值"<<"吸光度"<<"测量时间"<<"仪器状态"<<"报警类型"<<"消解时长"<<"斜率"<<"截距"<<"消解温度"<<"重启时间"<<"修正因子"<<"修正偏差"<<"标液1浓度"<<"标液1吸光度"<<"标液2浓度"<<"标液2吸光度"<<"exten_cn=3020";
    map["lc-water-b-2.1"]<<"蓝创B款"<<"是否有效测量值"<<"测量值"<<"吸光度"<<"取水时间"<<"仪器状态"<<"报警类型"<<"消解时长"<<"斜率"<<"截距"<<"消解温度"<<"重启时间"<<"修正因子"<<"修正偏差"<<"标液1浓度"<<"标液1吸光度"<<"标液2浓度"<<"标液2吸光度"<<"零点校准测量值"<<"零点校准结果"<<"零点校准时间"<<"校满校准结果"<<"校满校准时间"<<"校准方式"<<"自动校准间隔"<<"当前量程"<<"测量精度"<<"样品分析时间"<<"样品分析值"<<"样品分析吸光度值"<<"标样核查分析时间"<<"标样核查分析值"<<"标样核查分析吸光度值"<<"标样核查标准值"<<"核查相对误差标准值"<<"exten_cn=3020";
    map["lc-water-shandong"]<<"蓝创water山东协议"<<"水样浓度"<<"水样吸光度"<<"水样测量时间"<<"TODO"<<"零点浓度"<<"零点吸光度"<<"零点校准时间"<<"量程浓度"<<"量程吸光度"<<"量程校准时间"<<"量程"<<"斜率"<<"截距"<<"修正斜率"<<"修正截距"<<"消解时间"<<"消解温度"<<"标样浓度"<<"核查吸光度"<<"核查浓度"<<"核查偏差"<<"核查时间"<<"检测时间"<<"工作状态"<<"故障状态"<<"数据质量码"<<"报警状态"<<"核查结果";
    map["lpm1000"]<<"lpm1000"<<"TSP浓度值(1分钟浓度值单位ug/m^3)"<<"TSP浓度值(平均时间浓度值单位mg/m^3)"<<"采样流量(L/MIN)"<<"内部温度"<<"大气压(KPa)";
    map["nanjing-feiqi-flag"]<<"南京废气Flag";
    map["nengju-liuliangji"]<<"上海能巨电磁流量计"<<"瞬时流量"<<"累计流量";
    map["ozone-sensor"]<<"航天新气象-臭氧"<<"臭氧浓度"<<"电源电压"<<"内部温度"<<"内部气压";
    map["shenzhen-best"]<<"shenzhen-best"<<"污水压力"<<"进出口压差"<<"管道温度";
    map["tag-reader"]<<"标签阅读器";
    map["toc-4200"]<<"EDAD-0003_TOC-4200"<<"TOC换算浓度"<<"测量时间";
    map["weiser"]<<"伟思设备485口通信协议MOD_WS1"<<"累计雨量"<<"昨日雨量"<<"今日雨量";
    map["zsc-ix-nanjing"]<<"zcs-ix-nanjing"<<"采样器工作模式"<<"采样器报警"<<"采样桶状态"<<"留样仪状态"<<"留样桶状态"<<"总瓶数"<<"已留瓶数"<<"冷藏柜温度"<<"进样阀状态"<<"搅拌机状态"<<"排水阀状态"<<"留样阀状态"<<"进样泵状态"<<"送样泵状态"<<"留样泵状态"<<"门禁状态"<<"门禁开关"<<"最后一次开门时间"<<"最后一次开门密码查看"<<"管理员密码"<<"维护密码"<<"药品剩余量"<<"平行样数";
    map["zsc-vib"]<<"zsc-vib"<<"A桶状态"<<"B桶状态";



    return map;
}

QMap<QString,QString> Util::Uart_devicetypeNameMatch()
{
     QMap<QString,QString> map;

     QMap<QString,QStringList> refmap = Uart_devicetype();
     QMap<QString,QStringList>::iterator it = refmap.begin();
     while(it!=refmap.end())
     {
         map[it.key()]=it.value().at(0);
         it++;
     }



     map["int16"]="16位整型";
     map["long"]="长整型";
     map["long-inverse"]="长整型反向";
     map["float32"]="32位浮点型";
     map["float3-4321"]="浮点型4321";
     map["float32-inverse"]="32位浮点型反向";
     map["float64"]="64位浮点型";
     map["float64-inverse"]="64位浮点型反向";
     map["coils线圈"]="coils线圈";

     return map;
}

QMap<QString,QString> Util::Uart_facnameMatch()
{
    QMap<QString,QString> map;
    httpclinet h;
    QJsonObject jObj;

    if(h.get(DCM_FACTOR,jObj))
    {
        QJsonObject::iterator it = jObj.begin();
        while(it != jObj.end())
        {
            QString code = it.key();
            QJsonObject valueObj = it.value().toObject();
            QString name = valueObj.value("name").toString();
            map.insert(code,name);
            it++;
        }
    }

    return map;
}

