#ifndef USBUTILITY_H
#define USBUTILITY_H

#include <QObject>
#include <QWidget>
#include <QMap>
#include <QString>

class UsbUtility : public QObject
{
    Q_OBJECT
public:
    explicit UsbUtility(QObject *parent = nullptr);
    ~UsbUtility();

static QString matchDevName(QString);

signals:

};

#endif // USBUTILITY_H
