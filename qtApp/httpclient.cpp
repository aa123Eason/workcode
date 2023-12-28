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
    QString deviceid = "LC2023RK100041";
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

bool HttpClient::putdata(const QString &url, const QJsonObject &json) {
    QNetworkAccessManager manager;
    QUrl requestUrl(url);
    QNetworkRequest request(requestUrl);
    // 设置请求方法为PUT
    request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
    request.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, true);
    request.setRawHeader("Content-Type", "application/json");
    // 将QJsonObject转换为JSON字符串
    QJsonDocument doc(json);
    QByteArray dataToSend = doc.toJson();
    // 发送PUT请求
    QNetworkReply *reply = manager.put(request, dataToSend);
    QEventLoop loop;
    QObject::connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
    loop.exec(); // 等待请求完成
    // 检查响应状态码
    if (reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() >= 200 && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() < 300) {
        return true; // 成功
    } else {
        return false; // 失败
    }
}

bool HttpClient::postdata(const QString &url, const QJsonObject &json)
{
    QNetworkAccessManager manager;

    QUrl requestUrl(url);
    QNetworkRequest request(requestUrl);
    // 设置请求头，如果需要的话
    // request.setRawHeader("Content-Type", "application/json");
    // 将QJsonObject转换为JSON字符串
    QJsonDocument doc(json);
    QByteArray dataToSend = doc.toJson();
    // 发送POST请求
    QNetworkReply *reply = manager.post(request, dataToSend);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec(); // 等待请求完成
    // 检查响应状态码
    if (reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() >= 200 && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() < 300) {
        return true; // 成功
    } else {
        return false; // 失败
    }
}

