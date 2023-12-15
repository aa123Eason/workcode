#include "cfactor.h"
#include <QDebug>
CFactor::CFactor()
{
    m_Property = "0";
    m_FactorData = "-";
    m_DateTime = "-";
    m_Flag = false;
}

CFactor::~CFactor()
{
    // qDebug() << "   ==> CFactor removed!!" << m_FactorName;
}
