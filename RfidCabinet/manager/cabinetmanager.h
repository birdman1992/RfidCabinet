#ifndef CABINETMANAGER_H
#define CABINETMANAGER_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QStringList>
#include <qbytearray.h>
#include <QPoint>
#include <QSettings>
#include <QFile>
#include <QDebug>
#include "config.h"
#include "funcs/chineseletterhelper.h"
#include <Structs/userinfo.h>

enum CabState
{
    STATE_NO = 0,//无状态
    STATE_STORE = 1,//存状态
    STATE_FETCH = 2,//取状态
    STATE_REFUN = 3,//退货状态
    STATE_LIST = 4,//列表取货状态
    STATE_CHECK = 5,//盘点状态
    STATE_REBIND = 6,//重新绑定
};

class CabinetManager : public QObject
{
    Q_OBJECT
public:
    explicit CabinetManager(QObject *parent = NULL);
    static CabinetManager* manager();
    bool setServerAddress(QString addr);
    void setCabCtrlWorld(QPoint caseAddr, QPoint ctrlWord);
    void setCabinetId(QString id);
    void setAntPow(int pow);

    QPoint getCabCtrlWorld(QPoint caseAddr);
    QString getServerAddress();
    QString getCabinetId();
    int getAntPow();
    int getLockId(int col, int row);

    int cabinetColCount();
    int cabinetRowCount(int col);
    bool sleepFlagTimeout();
    void clearConfig();
    QString getPyCh(QString name);
    void addUser(UserInfo *user);
    UserInfo* checkUserLocal(QString userId);
    bool checkManagers(QString id);
    QString scanDataTrans(QString data);
    void saveFetchList(QByteArray data);

    CabState state;

private:
    static CabinetManager* m;

    QByteArray defaultCtrlWord(int col);
    void setConfig(QString key, QVariant value);
    QVariant getConfig(QString key, QVariant defaultRet);

    void removeConfig(QString path);
signals:

public slots:
};

#endif // CABINETMANAGER_H
