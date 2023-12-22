#include "factorinfo.h"

FactorInfo::FactorInfo()
{

}

FactorInfo::~FactorInfo()
{
}

void FactorInfo::printFactor()
{
    qDebug()<<__LINE__<<"Name"<<this->m_name<<endl;
    qDebug()<<__LINE__<<"Code"<<this->m_code<<endl;
    qDebug()<<__LINE__<<"Unit"<<this->m_unit<<endl;
    qDebug()<<__LINE__<<"Display"<<this->m_display<<endl;
    qDebug()<<__LINE__<<"Sort"<<this->m_Alias<<endl;
    qDebug()<<__LINE__<<"Upload"<<this->m_upload<<endl;
    qDebug()<<__LINE__<<"Max"<<this->m_RangeUpper<<endl;
    qDebug()<<__LINE__<<"Min"<<this->m_RangeLower<<endl;
    qDebug()<<__LINE__<<"Alarm upper"<<this->m_AlarmUpper<<endl;
    qDebug()<<__LINE__<<"Used"<<this->m_used<<endl;

}
