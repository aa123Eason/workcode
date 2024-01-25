#include "httpclient.h"

HttpClient::HttpClient(QObject *parent) : QObject(parent)
{

}

HttpClient::~HttpClient()
{

}


bool HttpClient::getdata(QString url, QJsonObject &json)
{
    // 创建网络访问管理器
    QNetworkAccessManager manager;
    // 设置网络请求
    //http://101.34.210.4:8899/LC2023RK100041/dcm/realtime_data
    QString deviceid = DEVICEID;
    QString fullUrl = "http://101.34.210.4:8899/"+deviceid+url;

    QUrl requestUrl(fullUrl);
    QNetworkRequest request(requestUrl);
    // 发送 GET 请求
    QNetworkReply *reply = manager.get(request);
    // 等待请求完成
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    // 检查回复是否成功
    if (reply->error() == QNetworkReply::NoError) {
        // 读取回复内容
        QByteArray responseData = reply->readAll();
        // 解析 JSON 文档
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        if (!jsonDoc.isNull()) {
            // 检查是否是 JSON 对象
            if (jsonDoc.isObject()) {
                // 将解析后的 JSON 对象赋值给输出参数
                json = jsonDoc.object();

                return true; // 返回成功状态
            } else {
                // 处理非 JSON 对象的情况（可选）
            }
        } else {
            // 处理 JSON 解析失败的情况（可选）
        }
    } else {
        // 处理网络错误（可选）
    }
    reply->deleteLater(); // 清理网络回复对象
    return false; // 返回失败状态
}

bool HttpClient::asyngetdata(QString url,QJsonObject &json)
{

    // 注意：由于Qt的网络操作是异步的，我们不能直接从函数中返回结果。
    // 因此，这里我们使用了一个局部的事件循环来等待网络操作完成。
    // 这并不是最佳实践，因为它会阻塞当前线程直到网络操作完成。
    // 在实际应用中，您应该使用信号和槽机制来处理异步操作的结果。
    // 但为了简化示例，这里使用了事件循环。
    //http://101.34.210.4:8899/LC2023RK100032/dcm/mninfo
    QNetworkAccessManager manager;
    QString deviceid = DEVICEID;
    QString fullUrl = "http://101.34.210.4:8899/"+deviceid+url;
    qDebug()<<__LINE__<<fullUrl<<endl;
    QUrl requestUrl(fullUrl);
    QNetworkReply *reply = manager.get(QNetworkRequest(requestUrl));
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start(3000); // 设置超时为5秒，以防止网络请求无限期地挂起
    loop.exec(); // 进入事件循环，等待网络操作完成或超时
    if (reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        return false; // 网络请求出错，返回false
    }
    QString responseString = reply->readAll();
    reply->deleteLater();
    QJsonDocument doc = QJsonDocument::fromJson(responseString.toUtf8());
    if (doc.isObject()) {
        if (doc.object().isEmpty()) {
            reply->abort();
            reply->close();
            return false; // 响应内容为空对象，返回false
        } else {
            json = doc.object(); // 将响应内容赋值给传入的json对象
            reply->abort();
            reply->close();
            return true; // 获取成功，返回true
        }
    } else {
        reply->abort();
        reply->close();
        return false; // 响应内容不是JSON对象，返回false
    }
}

bool HttpClient::putdata(const QString &url,const QJsonObject &datajson,QJsonObject &resjson) {
    QNetworkAccessManager manager;
    QString deviceid = DEVICEID;
    QString fullUrl = "http://101.34.210.4:8899/"+deviceid+url;
    qDebug()<<__LINE__<<fullUrl<<endl;
    QUrl requestUrl(fullUrl);
    QNetworkRequest request(requestUrl);
    // 设置请求方法为PUT
    request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
    request.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, true);
    request.setRawHeader("Content-Type", "application/json");
    // 将QJsonObject转换为JSON字符串
    QJsonDocument doc(datajson);
    QByteArray dataToSend = doc.toJson();
    // 发送PUT请求
    QNetworkReply *reply = manager.put(request, dataToSend);

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start(5000); // 设置超时为5秒，以防止网络请求无限期地挂起
    loop.exec(); // 进入事件循环，等待网络操作完成或超时
    if (reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        return false; // 网络请求出错，返回false
    }
    QString responseString = reply->readAll();
    reply->deleteLater();
    QJsonDocument doc1 = QJsonDocument::fromJson(responseString.toUtf8());
    if (doc1.isObject()) {
        if (doc1.object().isEmpty()) {
            return false; // 响应内容为空对象，返回false
        } else {
            resjson = doc1.object(); // 将响应内容赋值给传入的json对象
            return true; // 获取成功，返回true
        }
    } else {
        return false; // 响应内容不是JSON对象，返回false
    }
}

bool HttpClient::postdata(const QString &url, const QJsonObject &json,QJsonObject &resjson)
{
    QNetworkAccessManager manager;
    QString deviceid = DEVICEID;
    QString fullUrl = "http://101.34.210.4:8899/"+deviceid+url;
    qDebug()<<__LINE__<<fullUrl<<endl;
    QUrl requestUrl(fullUrl);
    QNetworkRequest request(requestUrl);
    // 设置请求头，如果需要的话
    // request.setRawHeader("Content-Type", "application/json");
    // 将QJsonObject转换为JSON字符串
    QJsonDocument doc(json);
    QByteArray dataToSend = doc.toJson();
    // 发送POST请求
    QNetworkReply *reply = manager.post(request, dataToSend);
    if(reply==nullptr)
    {
        return false;
    }
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start(5000); // 设置超时为5秒，以防止网络请求无限期地挂起
    loop.exec(); // 进入事件循环，等待网络操作完成或超时
    if (reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        return false; // 网络请求出错，返回false
    }
    QString responseString = reply->readAll();
    reply->deleteLater();
    QJsonDocument doc1 = QJsonDocument::fromJson(responseString.toUtf8());
    if (doc1.isObject()) {
        if (doc1.object().isEmpty()) {
            return false; // 响应内容为空对象，返回false
        } else {
            resjson = doc1.object(); // 将响应内容赋值给传入的json对象
            return true; // 获取成功，返回true
        }
    } else {
        return false; // 响应内容不是JSON对象，返回false
    }
}

bool HttpClient::asynpostdata(const QString &url,const QJsonObject &datajson,QJsonObject &resjson)
{
    QEventLoop eventloop;
    QTimer timer;
    timer.singleShot(3000, &eventloop, &QEventLoop::quit);

    QNetworkAccessManager manager;
    manager.setNetworkAccessible(QNetworkAccessManager::Accessible);
    manager.setProxy(QNetworkProxy::NoProxy);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
    QString deviceid = DEVICEID;
    QString fullUrl = "http://101.34.210.4:8899/"+deviceid+url;
    qDebug()<<__LINE__<<fullUrl<<endl;
    QUrl requestUrl(fullUrl);
    /*QNetworkRequest request(requestUrl)*/;
    request.setUrl(requestUrl);
    timer.start(3000);

//     将QJsonObject转换为JSON字符串
    QJsonDocument doc(datajson);
    QByteArray dataToSend = doc.toJson();
    QNetworkReply* reply = manager.post(request, dataToSend);
    connect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
    eventloop.exec(QEventLoop::ExcludeUserInputEvents); //开启事件循环
    QNetworkReply::NetworkError error;
    if (!timer.isActive())
    {
        //超时，未知状态
        disconnect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
        reply->abort();
        reply->deleteLater();
        error = QNetworkReply::NetworkError::TimeoutError;
        return false;
    }

    error = reply->error();
    QString errorString = reply->errorString();
    qDebug()<<__LINE__<<errorString<<endl;
    QByteArray bytArr = reply->readAll();
    qDebug()<<__LINE__<<endl;
    QJsonDocument jDoc = QJsonDocument::fromJson(bytArr);
    resjson = jDoc.object();
    qDebug()<<__LINE__<<endl;
    reply->abort();
    reply->deleteLater();
    qDebug()<<__LINE__<<endl;
    return true;
}

bool HttpClient::asynpostseqcode(QJsonObject &resjson)
{
    QEventLoop eventloop;
    QTimer timer;
    timer.singleShot(3000, &eventloop, &QEventLoop::quit);

    QNetworkAccessManager manager;
    manager.setNetworkAccessible(QNetworkAccessManager::Accessible);
    manager.setProxy(QNetworkProxy::NoProxy);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
    QString fullUrl = "http://101.34.210.4:8899/dcms/list";
    QUrl requestUrl(fullUrl);
    /*QNetworkRequest request(requestUrl)*/;
    request.setUrl(requestUrl);
    timer.start(3000);

    QJsonObject dataJson;
    QJsonDocument doc(dataJson);
    QByteArray dataToSend = doc.toJson();
    QNetworkReply* reply = manager.post(request, dataToSend);
    connect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
    eventloop.exec(QEventLoop::ExcludeUserInputEvents); //开启事件循环
    QNetworkReply::NetworkError error;
    if (!timer.isActive())
    {
        //超时，未知状态
        disconnect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
        reply->abort();
        reply->deleteLater();
        error = QNetworkReply::NetworkError::TimeoutError;
        return false;
    }

    error = reply->error();
    QString errorString = reply->errorString();
    qDebug()<<__LINE__<<errorString<<endl;
    QByteArray bytArr = reply->readAll();
    qDebug()<<__LINE__<<endl;
    QJsonDocument jDoc = QJsonDocument::fromJson(bytArr);
    resjson = jDoc.object();
    qDebug()<<__LINE__<<endl;
    reply->abort();
    reply->deleteLater();
    qDebug()<<__LINE__<<endl;
    return true;
}

