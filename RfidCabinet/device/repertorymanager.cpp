#include "repertorymanager.h"
#include "config.h"
#include <QDebug>

RepertoryManager::RepertoryManager(QObject *parent) : QObject(parent)
{

}

void RepertoryManager::rfidIn(GoodsInfo* info)
{
    qDebug()<<info->rfid;
    if(info->rfid.isEmpty())
        return;
    qDebug()<<"[rfidIn]"<<info->rfid<<info->name<<info->rfid;
    QSettings settings(getRepFile(info->antId), QSettings::IniFormat);
    settings.beginGroup(info->name);
    settings.setValue("name",info->name);
    settings.setValue("abbname",info->abbName);
    QString rfids = settings.value("rfids", QString()).toString();
    int goodsCount  = rfidAppend(rfids,QString(info->rfid));
    settings.setValue("num",goodsCount);
    settings.setValue("rfids",rfids);
    settings.endGroup();
    settings.sync();
}

void RepertoryManager::rfidOut(int antId, QString goodsId, QByteArray rfid)
{
    if(rfid.isEmpty())
        return;

    QSettings settings(getRepFile(antId), QSettings::IniFormat);
    QStringList goodsIdList = settings.childGroups();
    if(goodsIdList.indexOf(QString(rfid)) == -1)
        return;

    settings.beginGroup(goodsId);
    QString rfids = settings.value("rfids", QString()).toString();
    int goodsCount = rfidRemove(rfids, QString(rfid));
    settings.setValue("num",goodsCount);
    settings.endGroup();
    settings.sync();
}

QString RepertoryManager::getRepFile(int antId)
{
    return QString(FILE_CONFIG_REP) + QString("%1.rep").arg(antId);
}

int RepertoryManager::rfidAppend(QString& rfids, QString rfid)
{
    QStringList rfidList = rfids.split(" ");
    int index = rfidList.indexOf(rfid);
    if(index == -1)
    {
        rfidList<<rfid;
        rfids = rfidList.join(" ");
    }
    qDebug()<<"[rfidAppend]"<<rfidList.count();
    return rfidList.count();
}

int RepertoryManager::rfidRemove(QString &rfids, QString rfid)
{
    QStringList rfidList = rfids.split(" ");
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
