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

#ifndef CHART_H
#define CHART_H

#include <QtCharts/QChart>
#include <QtCore/QTimer>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLegend>
#include <QtCharts/QLegendMarker>
#include <QtCharts/QXYLegendMarker>
#include <QString>
#include <QMap>
#include "factorinfo.h"
#include <QDebug>
#include <QStringList>


QT_CHARTS_BEGIN_NAMESPACE
class QLineSeries;
class QSplineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

//![1]
class Chart: public QChart
{
    Q_OBJECT
public:
    Chart(QMap<QString,FactorInfo *> &map,QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
    virtual ~Chart();
    void getmapFactor(QMap<QString,FactorInfo *> &map);

public slots:
    void handleTimeout();

private:
    QTimer m_timer;
    QLineSeries *sYQWD=nullptr,*sYQLS=nullptr,*sYQYL=nullptr,*sYQSD=nullptr,*sYQHL=nullptr,*sBKLL=nullptr;
    QStringList m_titles;
    QValueAxis *m_axisX=nullptr;
    QValueAxis *m_axisY=nullptr;
    qreal m_step;
    qreal xYQWD,xYQLS,xYQYL,xYQSD,xYQHL,xBKLL;
    qreal yYQWD,yYQLS,yYQYL,yYQSD,yYQHL,yBKLL;
    QMap<QString,FactorInfo *> paramsMap;
};
//![1]

#endif /* CHART_H */
