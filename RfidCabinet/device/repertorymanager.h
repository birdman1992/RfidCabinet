#ifndef REPERTORYMANAGER_H
#define REPERTORYMANAGER_H

#include <QObject>
#include <QSettings>
#include <QStringList>
#include "Structs/cabinetinfo.h"
#include "Structs/epcinfo.h"

class RepertoryManager : public QObject
{
    Q_OBJECT
public:
    static RepertoryManager* manager();
    void clearRepertory();
    void rfidOut(EpcInfo *info);//rfid登记移除
    void rfidUpdate(EpcInfo *info, bool stateUpdate = false);//rfid新增
    void rfidUpdate(QString goodsId, QString epcId, int state);//rfid状态更新
    QList<EpcInfo*> epcTraversal();//遍历epc标签

private:
    explicit RepertoryManager(QObject *parent = NULL);
    static RepertoryManager* m;
    QString getRepFile(QPoint pos);
    QStringList getEpcs(QSettings &settings, QString group);
    int getGoodsCount(QSettings &settings, QString goodsId);
    int rfidAppend(QString& rfids, QString rfid);
    int rfidRemove(QString& rfids, QString rfid);

signals:

public slots:
};

#endif // REPERTORYMANAGER_H
