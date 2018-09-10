#ifndef CABINETMANAGER_H
#define CABINETMANAGER_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QStringList>
#include <qbytearray.h>
#include <QPoint>
#include "config.h"
#include "funcs/chineseletterhelper.h"

class CabinetManager : public QObject
{
    Q_OBJECT
public:
    explicit CabinetManager(QObject *parent = NULL);
    static CabinetManager* manager();
    bool setServerAddress(QString addr);
    void setCabCtrlWorld(QPoint caseAddr, QPoint ctrlWord);
    void setCabinetId(QString id);

    QPoint getCabCtrlWorld(QPoint caseAddr);
    QString getServerAddress();
    QString getCabinetId();
    int getLockId(int col, int row);

    int cabinetColCount();
    int cabinetRowCount(int col);
    int sleepFlagTimeout();
    void clearConfig();
    QString getPyCh(QString name);
    void addUser(QString id);
    bool checkManagers(QString id);
    QString scanDataTrans(QString data);
    void saveFetchList(QByteArray data);

private:
    static CabinetManager* m;

    QByteArray defaultCtrlWord(int col);
    void setConfig(QString key, QVariant value);
    QVariant getConfig(QString key, QVariant defaultRet);

signals:

public slots:
};

#endif // CABINETMANAGER_H
