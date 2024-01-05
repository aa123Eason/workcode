/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "chart.h"

#include <QtCore/QRandomGenerator>
#include <QtCore/QDebug>

Chart::Chart(QMap<QString,FactorInfo *> &map,QGraphicsItem *parent, Qt::WindowFlags wFlags):
    QChart(QChart::ChartTypeCartesian, parent, wFlags),
    sYQWD(0),sYQLS(0),sYQYL(0),sYQSD(0),sYQHL(0),sBKLL(0),
    m_axisX(new QValueAxis()),
    m_axisY(new QValueAxis()),
    m_step(0),
    xYQWD(0),xYQLS(0),xYQYL(0),xYQSD(0),xYQHL(0),xBKLL(0),
    yYQWD(0),yYQLS(0),yYQYL(0),yYQSD(0),yYQHL(0),yBKLL(0)
{

    getmapFactor(map);
    QObject::connect(&m_timer, &QTimer::timeout, this, &Chart::handleTimeout);
    m_timer.setInterval(5000);

    sYQWD = new QLineSeries(this);
    sYQLS = new QLineSeries(this);
    sYQYL = new QLineSeries(this);
    sYQSD = new QLineSeries(this);
    sYQHL = new QLineSeries(this);
    sYQWD = new QLineSeries(this);
    sBKLL = new QLineSeries(this);

    QPen pYQWD(Qt::red);pYQWD.setWidthF(1);
    QPen pYQLS(Qt::green);pYQLS.setWidthF(1.5);
    QPen pYQYL(Qt::blue);pYQYL.setWidthF(2);
    QPen pYQSD(Qt::cyan);pYQSD.setWidthF(2.5);
    QPen pYQHL(Qt::magenta);pYQHL.setWidthF(3);
    QPen pBKLL(Qt::darkGray);pBKLL.setWidthF(3.5);

    sYQWD->setPen(pYQWD);
    sYQLS->setPen(pYQLS);
    sYQYL->setPen(pYQYL);
    sYQSD->setPen(pYQSD);
    sYQHL->setPen(pYQHL);
    sBKLL->setPen(pBKLL);

    sYQWD->append(xYQWD, yYQWD);
    sYQLS->append(xYQLS, yYQLS);
    sYQYL->append(xYQYL, yYQYL);
    sYQSD->append(xYQSD, yYQSD);
    sYQHL->append(xYQHL, yYQHL);
    sBKLL->append(xBKLL, yBKLL);

    sYQWD->setName("烟气温度");
    sYQLS->setName("烟气流速");
    sYQYL->setName("烟气压力");
    sYQSD->setName("烟气湿度");
    sYQHL->setName("氧气含量");
    sBKLL->setName("标况流量");


    addSeries(sYQWD);
    addSeries(sYQLS);
    addSeries(sYQYL);
    addSeries(sYQSD);
    addSeries(sYQHL);
    addSeries(sBKLL);

    addAxis(m_axisX,Qt::AlignBottom);
    addAxis(m_axisY,Qt::AlignLeft);

    sYQWD->attachAxis(m_axisX);
    sYQLS->attachAxis(m_axisX);
    sYQYL->attachAxis(m_axisX);
    sYQSD->attachAxis(m_axisX);
    sYQHL->attachAxis(m_axisX);
    sBKLL->attachAxis(m_axisX);

    sYQWD->attachAxis(m_axisY);
    sYQLS->attachAxis(m_axisY);
    sYQYL->attachAxis(m_axisY);
    sYQSD->attachAxis(m_axisY);
    sYQHL->attachAxis(m_axisY);
    sBKLL->attachAxis(m_axisY);


    m_axisX->setTickCount(10);
    m_axisX->setRange(0, 100);
    m_axisY->setRange(-20, 100);

    m_axisX->setLabelsVisible(false);




    m_timer.start();
}

Chart::~Chart()
{
    m_timer.stop();
}

void Chart::handleTimeout()
{

    qreal x = 20;
    qreal y = (m_axisX->max() - m_axisX->min()) / m_axisX->tickCount();
//    xYQWD(0),xYQLS(0),xYQYL(0),xYQSD(0),xYQHL(0),xBKLL(0),
//    yYQWD(0),yYQLS(0),yYQYL(0),yYQSD(0),yYQHL(0),yBKLL(0)
    xYQWD += y;
    xYQLS += y;
    xYQYL += y;
    xYQSD += y;
    xYQHL += y;
    xBKLL += y;

    if(paramsMap.contains("烟气温度"))
        yYQWD = paramsMap["烟气温度"]->m_value.toDouble();
    if(paramsMap.contains("烟气流速"))
        yYQLS = paramsMap["烟气流速"]->m_value.toDouble();
    if(paramsMap.contains("烟气压力"))
        yYQYL = paramsMap["烟气压力"]->m_value.toDouble();
    if(paramsMap.contains("烟气湿度"))
        yYQSD = paramsMap["烟气湿度"]->m_value.toDouble();
    if(paramsMap.contains("氧气含量"))
        yYQHL = paramsMap["氧气含量"]->m_value.toDouble();
    if(paramsMap.contains("标况流量"))
        yBKLL = paramsMap["标况流量"]->m_value.toDouble();

    qDebug()<<__LINE__<<"烟气温度"<<yYQWD;
    qDebug()<<"烟气流速"<<yYQLS;
    qDebug()<<"烟气压力"<<yYQYL;
    qDebug()<<"烟气湿度"<<yYQSD;
    qDebug()<<"氧气含量"<<yYQHL;
    qDebug()<<"标况流量"<<yBKLL;

    QList<double> valuelist;
    valuelist<<yYQWD<<yYQLS<<yYQYL<<yYQSD<<yYQHL<<yBKLL;

    qreal minValue = -20,maxValue = 100;
    for(double value:valuelist)
    {
        if(minValue>value)
            minValue = value;

        if(maxValue<value)
            maxValue = value;
    }

    m_axisY->setRange(minValue, maxValue);

    sYQWD->append(xYQWD, yYQWD);
    sYQLS->append(xYQLS, yYQLS);
    sYQYL->append(xYQYL, yYQYL);
    sYQSD->append(xYQSD, yYQSD);
    sYQHL->append(xYQHL, yYQHL);
    sBKLL->append(xBKLL, yBKLL);

    scroll(x, 0);
//    if (xYQWD == 100 || xYQLS == 100 ||
//        xYQYL == 100 || xYQSD == 100 ||
//        xYQHL == 100 || xBKLL == 100)
//        m_timer.stop();
}

void Chart::getmapFactor(QMap<QString,FactorInfo *> &map)
{
    qDebug()<<__LINE__<<map<<endl;
    QStringList tmpList;
    tmpList<<"烟气温度"<<"烟气流速"<<"烟气压力"<<"烟气湿度"<<"氧气含量"<<"标况流量";

    for(int i=0;i<tmpList.count();++i)
    {
        QString paraName = tmpList[i];
        if(map.contains(paraName))
            paramsMap.insert(paraName,map[paraName]);
    }




}
