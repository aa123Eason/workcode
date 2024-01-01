#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QEventLoop>
#include <QTimer>
#include <QNetworkProxy>

class HttpClient : public QObject
{
    Q_OBJECT
public:
    explicit HttpClient(QObject *parent = nullptr);
    ~HttpClient();
    bool getdata(QString url, QJsonObject &json);
    bool asyngetdata(QString url, QJsonObject &json);
    bool putdata(const QString &url, const QJsonObject &datajson,QJsonObject &resjson);
    bool postdata(const QString &url, const QJsonObject &datajson,QJsonObject &resjson);
    bool asynpostdata(const QString &url,const QJsonObject &datajson,QJsonObject &resjson);

signals:

public slots:
};

#endif // HTTPCLIENT_H
