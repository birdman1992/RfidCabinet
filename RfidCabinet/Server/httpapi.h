#ifndef HTTPAPI_H
#define HTTPAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <qstringlist.h>
#include "Structs/rfidchangeinfo.h"
#include "Structs/cabinetinfo.h"

class HttpApi : public QObject
{
    Q_OBJECT
public:
    explicit HttpApi(QObject *parent = NULL);

private:
    QString token;
    bool needToken;
    QStringList listGoodsName;

    QNetworkAccessManager* manager;
    QNetworkReply* reply_token;
    QNetworkReply* reply_delete;

    void getToken(QString userName, QString passWd);
    void updateToken();
    void deleteToken();
    int getNameIndex(QByteArray qba);

signals:
    void newStoreList(QList<GoodsInfo*>);
    void newFetchList(QList<GoodsInfo*>);

public slots:
    void rfidStore(QList<rfidChangeInfo *>);
    void rfidFetch(QList<rfidChangeInfo *>);

private slots:
    void httpReply(QNetworkReply*);
    void checkReply(QNetworkReply* reply);
    void replyToken();
    void replyUpdateToken();
    void replyDeleteToken();
};

#endif // HTTPAPI_H
