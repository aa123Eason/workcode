#include "httpclinet.h"
#include <QEventLoop>

httpclinet::httpclinet()
{

}

bool httpclinet::get(QString api,QJsonArray &pJsonArray)
{
    QTimer timer;
    timer.setInterval(1000);  // 设置超时时间 30 秒
    timer.setSingleShot(true);  // 单次触发

    //生成对应的网络请求
    QNetworkRequest request;
    QString scheme = SCHEME;
    QString serverAddr = SERVER_IP;
    QString port = SERVER_PORT;
    QString requestHeader = scheme + QString("://") + serverAddr + QString(":") + port;
    QString fullRequest = requestHeader + QString(api);
    request.setUrl(QUrl(fullRequest));

    //发送Get请求
    QNetworkAccessManager manager;
    QNetworkReply *pReply = manager.get(request);

    QEventLoop loop;
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start();
    loop.exec(QEventLoop::ExcludeUserInputEvents);  // 启动事件循环

    if (timer.isActive()) {  // 处理响应
        timer.stop();
        if (pReply->error() != QNetworkReply::NoError) {
            // 错误处理
            qDebug() << "Error String : " << pReply->errorString();
            return false;
        } else {
            QVariant variant = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            int nStatusCode = variant.toInt();
            // 根据状态码做进一步数据处理
            //QByteArray bytes = pReply->readAll();
            qDebug() << "Status Code : " << nStatusCode;

            //解析返回的Json结果
            QByteArray replyData = pReply->readAll();
            QJsonParseError json_error;
            QJsonDocument jsonDoc(QJsonDocument::fromJson(replyData, &json_error));
            if(json_error.error != QJsonParseError::NoError)
            {
                return false;
            }
            pJsonArray = jsonDoc.array();
            return true;
        }
    } else {  // 处理超时
        QObject::disconnect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        pReply->abort();
        pReply->deleteLater();
        // qDebug() << "Timeout";
        return false;
    }

    //eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
}

bool httpclinet::get(QString api,QJsonObject &pJsonObj)
{
    QTimer timer;
    timer.setInterval(1000);  // 设置超时时间 30 秒
    timer.setSingleShot(true);  // 单次触发

    //生成对应的网络请求
    QNetworkRequest request;
    QString scheme = SCHEME;
    QString serverAddr = SERVER_IP;
    QString port = SERVER_PORT;
    QString requestHeader = scheme + QString("://") + serverAddr + QString(":") + port;
    QString fullRequest = requestHeader + QString(api);
    request.setUrl(QUrl(fullRequest));

    //发送Get请求
    QNetworkAccessManager manager;
    QNetworkReply *pReply = manager.get(request);

    QEventLoop loop;
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start();
    loop.exec(QEventLoop::ExcludeUserInputEvents);  // 启动事件循环

    if (timer.isActive()) {  // 处理响应
        timer.stop();
        if (pReply->error() != QNetworkReply::NoError) {
            // 错误处理
            qDebug() << "Error String : " << pReply->errorString();
            return false;
        } else {
            QVariant variant = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            int nStatusCode = variant.toInt();
            // 根据状态码做进一步数据处理
            //QByteArray bytes = pReply->readAll();
            qDebug() << "Status Code : " << nStatusCode;

            //解析返回的Json结果
            QByteArray replyData = pReply->readAll();
            QJsonParseError json_error;
            QJsonDocument jsonDoc(QJsonDocument::fromJson(replyData, &json_error));
            if(json_error.error != QJsonParseError::NoError)
            {
                return false;
            }
            pJsonObj = jsonDoc.object();
            return true;
        }
    } else {  // 处理超时
        QObject::disconnect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        pReply->abort();
        pReply->deleteLater();
        // qDebug() << "Timeout";
        return false;
    }
}

bool httpclinet::post(QString api,QJsonObject &pJsonObj)
{
    QTimer timer;
    timer.setInterval(1000);  // 设置超时时间 30 秒
    timer.setSingleShot(true);  // 单次触发

    //生成对应的网络请求
    QNetworkRequest request;
    QString scheme = SCHEME;
    QString serverAddr = SERVER_IP;
    QString port = SERVER_PORT;
    QString requestHeader = scheme + QString("://") + serverAddr + QString(":") + port;
    QString fullRequest = requestHeader + QString(api);
    request.setUrl(QUrl(fullRequest));

    QJsonDocument body(pJsonObj);

    // Connection
    const QByteArray connection = QByteArrayLiteral("Connection");
    const QByteArray connectionValue = QByteArrayLiteral("Connection");
    // Content-Type
    const QByteArray content = QByteArrayLiteral("Content-Type");
    const QByteArray contentValue = QByteArrayLiteral("application/json;charset=UTF-8");
    // token
    const QByteArray token = QByteArrayLiteral("Authorization");
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentValue);
    request.setRawHeader(connection, connectionValue);
    request.setRawHeader(token, TOKEN);
    QNetworkAccessManager *Manager = new QNetworkAccessManager();
    QNetworkReply *pReply = Manager->post(request, body.toJson());

    QEventLoop loop;
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start();
    loop.exec(QEventLoop::ExcludeUserInputEvents);  // 启动事件循环

    if (timer.isActive()) {  // 处理响应
        timer.stop();
        if (pReply->error() != QNetworkReply::NoError) {
            // 错误处理
            qDebug() << "Error String : " << pReply->errorString();
            return false;
        } else {
            QVariant variant = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            int nStatusCode = variant.toInt();
            // 根据状态码做进一步数据处理
            //QByteArray bytes = pReply->readAll();
            qDebug() << "Status Code : " << nStatusCode;

            //解析返回的Json结果
            QByteArray replyData = pReply->readAll();

            // 串口调试 回复 非json格式

            QJsonParseError json_error;
            QJsonDocument jsonDoc(QJsonDocument::fromJson(replyData, &json_error));
            if(json_error.error != QJsonParseError::NoError)
            {
                return false;
            }
            pJsonObj = jsonDoc.object();

            qDebug() << "post reply: " << pJsonObj;
            return true;
        }
    } else {  // 处理超时
        QObject::disconnect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        pReply->abort();
        pReply->deleteLater();
        // qDebug() << "Timeout";
        return false;
    }

}
bool httpclinet::post(QString api,QJsonObject &pJsonObj,QJsonObject &pJsonReply)
{
    QTimer timer;
    timer.setInterval(1000);  // 设置超时时间 30 秒
    timer.setSingleShot(true);  // 单次触发

    //生成对应的网络请求
    QNetworkRequest request;
    QString scheme = SCHEME;
    QString serverAddr = SERVER_IP;
    QString port = SERVER_PORT;
    QString requestHeader = scheme + QString("://") + serverAddr + QString(":") + port;
    QString fullRequest = requestHeader + QString(api);
    request.setUrl(QUrl(fullRequest));

    QJsonDocument body(pJsonObj);

    // Connection
    const QByteArray connection = QByteArrayLiteral("Connection");
    const QByteArray connectionValue = QByteArrayLiteral("Connection");
    // Content-Type
    const QByteArray content = QByteArrayLiteral("Content-Type");
    const QByteArray contentValue = QByteArrayLiteral("application/json;charset=UTF-8");
    // token
    const QByteArray token = QByteArrayLiteral("Authorization");
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentValue);
    request.setRawHeader(connection, connectionValue);
    request.setRawHeader(token, TOKEN);
    QNetworkAccessManager *Manager = new QNetworkAccessManager();
    QNetworkReply *pReply = Manager->post(request, body.toJson());

    QEventLoop loop;
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start();
    loop.exec(QEventLoop::ExcludeUserInputEvents);  // 启动事件循环

    if (timer.isActive()) {  // 处理响应
        timer.stop();
        if (pReply->error() != QNetworkReply::NoError) {
            // 错误处理
            qDebug() << "Error String : " << pReply->errorString();
            return false;
        } else {
            QVariant variant = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            int nStatusCode = variant.toInt();
            // 根据状态码做进一步数据处理
            //QByteArray bytes = pReply->readAll();
            qDebug() << "Status Code : " << nStatusCode;

            //解析返回的Json结果
            QByteArray replyData = pReply->readAll();

            // 串口调试 回复 非json格式

            QJsonParseError json_error;
            QJsonDocument jsonDoc(QJsonDocument::fromJson(replyData, &json_error));
            if(json_error.error != QJsonParseError::NoError)
            {
                return false;
            }
            pJsonReply = jsonDoc.object();
            return true;
        }
    } else {  // 处理超时
        QObject::disconnect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        pReply->abort();
        pReply->deleteLater();
        // qDebug() << "Timeout";
        return false;
    }

}


bool httpclinet::put(QString api,QJsonObject &pJsonObj)
{
    QTimer timer;
    timer.setInterval(1000);  // 设置超时时间 30 秒
    timer.setSingleShot(true);  // 单次触发

    //生成对应的网络请求
    QNetworkRequest request;
    QString scheme = SCHEME;
    QString serverAddr = SERVER_IP;
    QString port = SERVER_PORT;
    QString requestHeader = scheme + QString("://") + serverAddr + QString(":") + port;
    QString fullRequest = requestHeader + QString(api);
    request.setUrl(QUrl(fullRequest));

    QJsonDocument body(pJsonObj);

    // Connection
    const QByteArray connection = QByteArrayLiteral("Connection");
    const QByteArray connectionValue = QByteArrayLiteral("Connection");
    // Content-Type
    const QByteArray content = QByteArrayLiteral("Content-Type");
    const QByteArray contentValue = QByteArrayLiteral("application/json;charset=UTF-8");
    // token
    const QByteArray token = QByteArrayLiteral("Authorization");
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentValue);
    request.setRawHeader(connection, connectionValue);
    request.setRawHeader(token, TOKEN);
    QNetworkAccessManager *Manager = new QNetworkAccessManager();
    QNetworkReply *pReply = Manager->put(request, body.toJson());

    QEventLoop loop;
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start();
    loop.exec(QEventLoop::ExcludeUserInputEvents);  // 启动事件循环

    if (timer.isActive()) {  // 处理响应
        timer.stop();
        if (pReply->error() != QNetworkReply::NoError) {
            // 错误处理
            qDebug() << "Error String : " << pReply->errorString();
            return false;
        } else {
            QVariant variant = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            int nStatusCode = variant.toInt();
            // 根据状态码做进一步数据处理
            //QByteArray bytes = pReply->readAll();
            qDebug() << "Status Code : " << nStatusCode;

            //解析返回的Json结果
            QByteArray replyData = pReply->readAll();

            QJsonParseError json_error;
            QJsonDocument jsonDoc(QJsonDocument::fromJson(replyData, &json_error));
            if(json_error.error != QJsonParseError::NoError)
            {
                return false;
            }
            pJsonObj = jsonDoc.object();

            return true;
        }
    } else {  // 处理超时
        QObject::disconnect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        pReply->abort();
        pReply->deleteLater();
        // qDebug() << "Timeout";
        return false;
    }

    //eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
}

bool httpclinet::deleteSource(QString api,QString item)
{
    QTimer timer;
    timer.setInterval(1000);  // 设置超时时间 30 秒
    timer.setSingleShot(true);  // 单次触发

    //生成对应的网络请求
    QNetworkRequest request;
    QString scheme = SCHEME;
    QString serverAddr = SERVER_IP;
    QString port = SERVER_PORT;
    QString requestHeader = scheme + QString("://") + serverAddr + QString(":") + port;
    QString fullRequest = requestHeader + QString(api);
    request.setUrl(QUrl(fullRequest+item));

    // Connection
    const QByteArray connection = QByteArrayLiteral("Connection");
    const QByteArray connectionValue = QByteArrayLiteral("Connection");
    // Content-Type
    const QByteArray content = QByteArrayLiteral("Content-Type");
    const QByteArray contentValue = QByteArrayLiteral("application/json;charset=UTF-8");
    // token
    const QByteArray token = QByteArrayLiteral("Authorization");
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentValue);
    request.setRawHeader(connection, connectionValue);
    request.setRawHeader(token, TOKEN);
    QNetworkAccessManager *Manager = new QNetworkAccessManager();
    QNetworkReply *pReply = Manager->deleteResource(request);

    QEventLoop loop;
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start();
    loop.exec(QEventLoop::ExcludeUserInputEvents);  // 启动事件循环

    if (timer.isActive()) {  // 处理响应
        timer.stop();
        if (pReply->error() != QNetworkReply::NoError) {
            // 错误处理
            qDebug() << "Error String : " << pReply->errorString();
            return false;
        } else {
            QVariant variant = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            int nStatusCode = variant.toInt();
            // 根据状态码做进一步数据处理
            //QByteArray bytes = pReply->readAll();
            qDebug() << "Status Code : " << nStatusCode;

            //解析返回的Json结果
            // QByteArray replyData = pReply->readAll();
            return true;
        }
    } else {  // 处理超时
        QObject::disconnect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        pReply->abort();
        pReply->deleteLater();
        // qDebug() << "Timeout";
        return false;
    }

    //eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

}

/*

QTimer timer;
timer.setInterval(30000);  // 设置超时时间 30 秒
timer.setSingleShot(true);  // 单次触发

// 请求 Qt 官网
QNetworkAccessManager manager;
QNetworkRequest request;
request.setUrl(QUrl("http://qt-project.org"));
request.setRawHeader("User-Agent", "MyOwnBrowser 1.0");

QNetworkReply *pReply = manager.get(request);

QEventLoop loop;
connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
connect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
timer.start();
loop.exec();  // 启动事件循环

if (timer.isActive()) {  // 处理响应
    timer.stop();
    if (pReply->error() != QNetworkReply::NoError) {
        // 错误处理
        qDebug() << "Error String : " << pReply->errorString();
    } else {
        QVariant variant = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        int nStatusCode = variant.toInt();
        // 根据状态码做进一步数据处理
        //QByteArray bytes = pReply->readAll();
        qDebug() << "Status Code : " << nStatusCode;
    }
} else {  // 处理超时
    disconnect(pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    pReply->abort();
    pReply->deleteLater();
    qDebug() << "Timeout";
}
*/

