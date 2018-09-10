#include "repertorymanager.h"
#include "config.h"
#include <QDebug>

RepertoryManager::RepertoryManager(QObject *parent) : QObject(parent)
{

}

void RepertoryManager::rfidIn(GoodsInfo* info)
{
    if(info->rfid.isEmpty())
        return;
//    qDebug()<<"[RepertoryManager::rfidIn]"<<info->rfid<<info->name;
    QSettings settings(getRepFile(info->pos), QSettings::IniFormat);
    settings.beginGroup(info->name);
    settings.setValue("name",info->name);
    settings.setValue("abbname",info->abbName);
    QString rfids = settings.value("rfids", QString()).toString();
//    qDebug()<<rfids;
    int goodsCount  = rfidAppend(rfids,QString(info->rfid));
    settings.setValue("num",goodsCount);
    settings.setValue("rfids",rfids);
    settings.endGroup();
    settings.sync();
}

void RepertoryManager::rfidOut(GoodsInfo* info)
{
    if(info->rfid.isEmpty())
        return;
//    qDebug()<<"[RepertoryManager::rfidOut]"<<info->rfid<<info->name;
    QSettings settings(getRepFile(info->pos), QSettings::IniFormat);
    settings.beginGroup(info->name);
//    settings.setValue("name",info->name);
//    settings.setValue("abbname",info->abbName);
    QString rfids = settings.value("rfids", QString()).toString();
    int goodsCount  = rfidRemove(rfids,QString(info->rfid));
    settings.setValue("num",goodsCount);
    settings.setValue("rfids",rfids);
    settings.endGroup();
    settings.sync();
}

QString RepertoryManager::getRepFile(QPoint pos)
{
    return QString(FILE_CONFIG_REP) + QString("%1#%2.rep").arg(pos.x()).arg(pos.y());
}

int RepertoryManager::rfidAppend(QString& rfids, QString rfid)
{
    QStringList rfidList = rfids.split(" ", QString::SkipEmptyParts);
    int index = rfidList.indexOf(rfid);
    if(index == -1)
    {
        rfidList<<rfid;
        rfids = rfidList.join(" ");
    }
//    qDebug()<<"[rfidAppend]"<<rfidList.count();
    return rfidList.count();
}

int RepertoryManager::rfidRemove(QString &rfids, QString rfid)
{
    QStringList rfidList = rfids.split(" ", QString::SkipEmptyParts);
    if(rfidList.isEmpty())
        return 0;

    int index = rfidList.indexOf(rfid);
    if(index == -1)
    {
        rfids = rfidList.join(" ");
    }
    else
    {
        rfidList.removeAt(index);
        rfids = rfidList.join(" ");
    }
    return rfidList.count();
}
