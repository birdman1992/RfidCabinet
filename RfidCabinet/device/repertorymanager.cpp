#include "repertorymanager.h"
#include "config.h"
#include <QDebug>

RepertoryManager* RepertoryManager::m = new RepertoryManager;

RepertoryManager::RepertoryManager(QObject *parent) : QObject(parent)
{

}

RepertoryManager *RepertoryManager::manager()
{
    return m;
}

void RepertoryManager::clearRepertory()
{

}

void RepertoryManager::rfidUpdate(EpcInfo *info, bool stateUpdate)
{
//    if(info->rfid.isEmpty())
//        return;
//    qDebug()<<"[RepertoryManager::rfidIn]"<<info->rfid<<info->name;
    QSettings settings(getRepFile(info->pos), QSettings::IniFormat);
    QString group = info->goodsId;
    if(!stateUpdate)
    {
        settings.setValue(QString("%1/name").arg(group),info->goodsName);
        settings.setValue(QString("%1/abbname").arg(group),info->abbName);
        int goodsCount = getGoodsCount(settings, group);
        settings.setValue(QString("%1/num").arg(group), goodsCount);
    }
    qDebug()<<"[rfidUpdate]"<<info->epcId<<info->epcState;
    settings.setValue(QString("%1/%2").arg(group).arg(info->epcId), info->epcState);
    settings.sync();
}

void RepertoryManager::rfidUpdate(QString goodsId, QString epcId, int state)
{
    QSettings settings(getRepFile(QPoint()), QSettings::IniFormat);
    QString group = goodsId;
    settings.setValue(QString("%1/%2").arg(group).arg(epcId), state);
    int goodsCount = getGoodsCount(settings, group);
    settings.setValue(QString("%1/num").arg(group), goodsCount);
    settings.sync();
}

QList<EpcInfo *> RepertoryManager::epcTraversal()
{
    QPoint pos = QPoint(0, 0);
    QSettings settings(getRepFile(pos), QSettings::IniFormat);
    QStringList groups = settings.childGroups();
    QList<EpcInfo*> list_epc;

    foreach(QString group, groups)
    {
        QStringList epcs = getEpcs(settings, group);
        if(!epcs.isEmpty())
        {
            foreach(QString epc, epcs)
            {
                EpcInfo* info = new EpcInfo();
                info->goodsId = group;
                info->goodsName = settings.value(QString("%1/name").arg(group), QString()).toString();
                info->abbName = settings.value(QString("%1/abbname").arg(group), QString()).toString();
                info->epcState = settings.value(QString("%1/%2").arg(group).arg(epc), 0).toInt();
                info->epcId = epc;
                info->pos = pos;
                list_epc<<info;
            }
        }
    }
    return list_epc;
}

void RepertoryManager::rfidOut(EpcInfo *info)
{
    QSettings settings(getRepFile(info->pos), QSettings::IniFormat);
    QString group = info->goodsId;
//    settings.setValue(QString("%1/name").arg(group),info->name);
//    settings.setValue(QString("%1/abbname").arg(group),info->abbName);
    settings.remove(QString("%1/%2").arg(group).arg(info->epcId));
    int goodsCount = getGoodsCount(settings, group);
//    bool delFlag = (goodsCount <= 0);

//    if(delFlag)
//        settings.remove(group);
    settings.sync();
}

//current output 0#0
QString RepertoryManager::getRepFile(QPoint )
{
    return QString(FILE_CONFIG_REP) + QString("%1#%2.rep").arg(0).arg(0);
}

QStringList RepertoryManager::getEpcs(QSettings& settings, QString group)
{
    QStringList ret;
    settings.beginGroup(group);
    ret = settings.childKeys();
    ret.removeOne("name");
    ret.removeOne("abbname");
    ret.removeOne("num");
    settings.endGroup();

    return ret;
}

int RepertoryManager::getGoodsCount(QSettings &settings, QString goodsId)
{
    settings.beginGroup(goodsId);
    int goodsCount = 0;
    QStringList rfidList = settings.childKeys();
    rfidList.removeOne("name");
    rfidList.removeOne("abbname");
    rfidList.removeOne("num");

    foreach(QString rfid, rfidList)
    {
        int rfidState = settings.value(rfid, 0).toInt();
        if(rfidState & 0x02)//被扫描过的状态
        {
            goodsCount++;
        }
    }
    settings.setValue("num", goodsCount);
    settings.endGroup();
    return goodsCount;
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
