#ifndef HISTORYCHARTVIEW_H
#define HISTORYCHARTVIEW_H

#include <QWidget>
#include <QString>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStringList>
#include <QMap>
#include <QMessageBox>
#include <QDateTime>
#include <QFile>
#include <QByteArray>
#include <QTime>

#include <QtCharts/QSplineSeries>
#include <QtCharts/QLegend>
#include <QtCharts/QLegendMarker>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QXYLegendMarker>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlResult>

#include "common.h"
#include "factorinfo.h"

QT_CHARTS_BEGIN_NAMESPACE
class QLineSeries;
class QSplineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE


namespace Ui {
class HistoryChartView;
}

class HistoryChartView : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryChartView(QSqlDatabase &db,QWidget *parent = nullptr);
    ~HistoryChartView();
    void widgetInit();
    void chartInit();
    void getLocalDB(QSqlDatabase &);

    void connectevent();
    QString loadStyleSheet(QString qsspath);
    void paintCharts();
    QColor randomColor();

    static QMap<QString,QString> facNameMap();

signals:


public slots:
    void on_comboboxDTType_currentChanged(const QString &text);
    void on_pushbutton_query();
    void onReceiveGlobalMapAndList(QStringList &g_FactorsNameList,QMap<QString, FactorInfo*> &map_Factors);

private:
    Ui::HistoryChartView *ui;
    QMap<QString,QCheckBox *> legendBoxMap;
    QMap<QString,FactorInfo *> selFactorsMap;
    QSqlDatabase curdb;
    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *chartlayout = new QHBoxLayout();
    QStringList gFactorsNameList;
    QStringList dateTimeList;
    QMap<QString, FactorInfo*> mapFactors;
    QMap<QString,QList<qreal>> mapQueryResult;
    QValueAxis *m_axisX=nullptr;
    QValueAxis *m_axisY=nullptr;
    QChart *chart = nullptr;
    QString queryFormat;
};

#endif // HISTORYCHARTVIEW_H
