#ifndef CUPLOADSETTING_H
#define CUPLOADSETTING_H
#include "common.h"

class CUploadSetting
{
private:
    QString m_MN;
    QString m_IP;
    QString m_Port;
    QString m_Passwd;
    QString m_Timeout;
    QString m_MinInternal;
    QString m_UpInternal;
    QString m_Retry;
    bool m_Heartbreak;
    bool m_RtUpload;

public:
    CUploadSetting();

    QString get_mn(){return m_MN;}
    QString get_ip(){return m_IP;}
    QString get_port(){return m_Port;}
    QString get_passwd(){return m_Passwd;}
    QString get_timeout(){return m_Timeout;}
    QString get_min_internal(){return m_MinInternal;}
    QString get_upload_internal(){return m_UpInternal;}
    QString get_retry(){return m_Retry;}
    bool get_heartbreak(){return m_Heartbreak;}
    bool get_rt_upload(){return m_RtUpload;}

    void set_mn(QString pTemp){m_MN = pTemp;}
    void set_ip(QString pTemp){m_IP = pTemp;}
    void set_port(QString pTemp){m_Port = pTemp;}
    void set_passwd(QString pTemp){m_Passwd = pTemp;}
    void set_timeout(QString pTemp){m_Timeout = pTemp;}
    void set_min_internal(QString pTemp){m_MinInternal = pTemp;}
    void set_uplaod_internal(QString pTemp){m_UpInternal = pTemp;}
    void set_retry(QString pTemp){m_Retry = pTemp;}
    void set_heartbreak(bool pTemp){m_Heartbreak = pTemp;}
    void set_rt_upload(bool pTemp){m_RtUpload = pTemp;}
};

#endif // CUPLOADSETTING_H
