#include "httpapi.h"
#include <QNetworkRequest>
#include <qstring.h>
#include <QDebug>
#include "Json/cJSON.h"

#define BASE_URL "http://10.0.3.9:8081"
#define API_TOKEN "/api/token"

HttpApi::HttpApi(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    reply_token = NULL;
    reply_delete = NULL;
//    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpReply(QNetworkReply*)));
//    getToken("13787419941","123456");
}

void HttpApi::getToken(QString userName, QString passWd)
{
    needToken = true;
    QNetworkRequest request;
    request.setUrl(QString(BASE_URL)+QString(API_TOKEN));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    QByteArray postData = QByteArray("password=123456&username=13787419941");
    checkReply(reply_token);
    reply_token = manager->post(request, postData);
    connect(reply_token, SIGNAL(readyRead()), this, SLOT(replyToken()));
}

void HttpApi::updateToken()
{
    QByteArray Authorization = "Bearer "+token.toLocal8Bit();

    QNetworkRequest request;
    request.setUrl(QString(BASE_URL)+QString(API_TOKEN));
    request.setRawHeader("Authorization",Authorization);
    QByteArray postData = QByteArray();
    checkReply(reply_token);
    reply_token = manager->put(request, postData);

    connect(reply_token, SIGNAL(readyRead()), this, SLOT(replyUpdateToken()));
}

void HttpApi::deleteToken()
{
    QByteArray Authorization = "Bearer "+token.toLocal8Bit();

    QNetworkRequest request;
    request.setUrl(QString(BASE_URL)+QString(API_TOKEN));
    request.setRawHeader("Authorization",Authorization);
//    request.setAttribute(QNetworkRequest::CustomVerbAttribute, QVariant());
    checkReply(reply_delete);
//    reply_delete = manager->deleteResource(request);
    reply_delete = manager->sendCustomRequest(request,QByteArray("DELETE"));
    connect(reply_delete, SIGNAL(readyRead()), this, SLOT(replyDeleteToken()));
}

void HttpApi::rfidStore(int antId, QByteArray rfid)
{

}

void HttpApi::httpReply(QNetworkReply *reply)
{
    qDebug()<<reply->readAll();
}

void HttpApi::checkReply(QNetworkReply *reply)
{
    if(reply == NULL)
        return;
    reply->deleteLater();
    return;
}

void HttpApi::replyToken()
{
    if(reply_token == NULL)
        return;

    QByteArray qba = reply_token->readAll();
    qDebug()<<"[replyToken]"<<qba;

    cJSON* cJson = cJSON_Parse(qba.data());
    if(cJson == NULL)
        return;

    cJSON* jsonObj = cJSON_GetObjectItem(cJson, "data");
    if(jsonObj == NULL)
    {
        return;
    }

    QString jsonToken = QString(cJSON_GetObjectItem(jsonObj,"token")->valuestring);
//    qDebug()<<"[token]"<<jsonToken;
    token = jsonToken;

    updateToken();//test
}

void HttpApi::replyUpdateToken()
{
    if(reply_token == NULL)
        return;

    QByteArray qba = reply_token->readAll();
    qDebug()<<"[replyUpdateToken]"<<qba;

    cJSON* cJson = cJSON_Parse(qba.data());
    if(cJson == NULL)
        return;

    cJSON* jsonObj = cJSON_GetObjectItem(cJson, "data");
    if(jsonObj == NULL)
    {
        return;
    }

    QString jsonToken = QString(cJSON_GetObjectItem(jsonObj,"token")->valuestring);
//    qDebug()<<"[token]"<<jsonToken;
    token = jsonToken;

    deleteToken();
}

void HttpApi::replyDeleteToken()
{
    if(reply_delete == NULL)
        return;

    QByteArray qba = reply_delete->readAll();
    qDebug()<<"[replyDeleteToken]"<<qba;
}
