#ifndef EDITRCDEVICE_H
#define EDITRCDEVICE_H



#include <QWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QCheckBox>
#include <QLabel>
#include <QFont>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QByteArray>
#include <QJsonObject>
#include "httpclinet.h"
#include "common.h"
#include "util.h"
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QGridLayout>
#include <QEvent>
#include <QMouseEvent>

namespace Ui {
class EditRCDevice;
}

class EditRCDevice : public QWidget
{
    Q_OBJECT

public:
    explicit EditRCDevice(QString facname,QWidget *parent = nullptr);
    ~EditRCDevice();
    void init();
    void connectevent();
    void addTimeCks(QList<QCheckBox *> &timecks,QGridLayout &timeset,QWidget *w);
    void loadtmcksState(QString porName);

signals:
    void sendFinished(bool);

protected:
    bool eventFilter(QObject *watched=NULL, QEvent *event=NULL);

public slots:
    void onSave();


private:
    Ui::EditRCDevice *ui;
    QString name;
    QFont ckFont;
    QList<QCheckBox *> timecks;
    QList<QCheckBox *> typecks;
    QMap<QString,QStringList> selckMap;
    QStringList portsList;
    QStringList cmdlist;
    QGridLayout al1,al2,al3,al4;
    QMap<QString,QString> facnameMap;
    Util util;
    httpclinet pClient;
    QString cmdinfofile;
//    localKeyboard *kb = nullptr;
};

#endif // EDITRCDEVICE_H
