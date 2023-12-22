#ifndef FACTORINFO_H
#define FACTORINFO_H

#include <QObject>
#include <QString>
#include <QDebug>

class FactorInfo;

class FactorInfo
{

public:

    FactorInfo();
    ~FactorInfo();

    QString m_name;
    QString m_code;
    QString m_value;
    QString m_state;
    QString m_unit;
    bool m_display;
    bool m_upload;
    uint16_t m_Chan;
    float m_RangeUpper;
    float m_RangeLower;
    float m_AlarmUpper;
    float m_LC;
    QString m_Alias;
    bool m_used;

    void printFactor();
};

#endif // FACTORINFO_H
