#ifndef HTTPAPI_H
#define HTTPAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class HttpApi : public QObject
{
    Q_OBJECT
public:
    explicit HttpApi(QObject *parent = NULL);

private:
    QString token;
    bool needToken;

    QNetworkAccessManager* manager;
    QNetworkReply* reply_token;
    QNetworkReply* reply_delete;

    void getToken(QString userName, QString passWd);
    void updateToken();
    void deleteToken();

signals:

public slots:

private slots:
    void httpReply(QNetworkReply*);
    void checkReply(QNetworkReply* reply);
    void replyToken();
    void replyUpdateToken();
    void replyDeleteToken();
};

#endif // HTTPAPI_H
