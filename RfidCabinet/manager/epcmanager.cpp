#include "epcmanager.h"
#include <QDebug>
#include "Json/cJSON.h"

EpcManager* EpcManager::m = new EpcManager;
EpcManager *EpcManager::manager()
{
//    activePos = QPoint(0, 0);
    return m;
}

void EpcManager::setActiveCab(QPoint pos)
{
    activePos = pos;
}

void EpcManager::appendWatchList(QStringList l)
{
    QString rfid;
    foreach(rfid, l)
    {
        if(!hash_epc.contains(rfid))
        {
            hash_epc.insert(rfid, 0x01);//插入未被扫描的新物品
            newList.append(l);
            qDebug()<<"[append watch list]"<<rfid;
//            qDebug()<<hash_epc[rfid];
        }
        else
        {
            hash_epc[rfid] = 0x01;
        }
    }
}

void EpcManager::appendWatchList(QString l)
{
    if(hash_epc.contains(l))
    {
        hash_epc[l] = 0x01;
        return;
    }
    qDebug()<<"[append watch list]"<<l;
    hash_epc.insert(l, 0x01);//插入未被扫描的新物品
//    qDebug()<<hash_epc[l];
    newList.append(l);
}

void EpcManager::removeWatchList(QStringList l)
{
    QString rfid;
    foreach (rfid, l)
    {
        hash_epc.remove(rfid);
    }
}

void EpcManager::removeWatchList(QString l)
{
    hash_epc.remove(l);
}

/*
|0|1|2|3|4|5|6|7|
   0:1
0.旧物品:新物品
1.未被扫描:已被扫描
2.无标记:暂时取走
*/
void EpcManager::rfidClearing()
{
    QHash<QString, int>::const_iterator i;

    for( i=hash_epc.constBegin(); i!=hash_epc.constEnd(); ++i)
    {qDebug("1");
        EpcInfo* info = map_epc[i.key()];qDebug("2");
        info->epcState = i.value();
        qDebug()<<"[rfidClearing]"<<i.key()<<i.value();

        if((i.value() & 0x03) == 0x03)//被扫描的新物品
        {qDebug("A");
            foreach(GoodsList* list, storeList)
            {
                if(list->storeRfid(i.key()))
                    break;
            }
            hash_epc[i.key()] &= 0xfe;//清空新物品标志
        }
        else if((i.value() & 0x07) == 0x00)//未被扫描的旧物品
        {
            qDebug("B");
            if(!delList.contains(i.key()))
            {
                delList<<i.key();
//                hash_epc[i.key()] |= 0x04;//标记为已取走
            }
        }
        else if(((i.value() & 0x06) == 0x06))//被扫描且被标记为取走的物品
        {
            qDebug("C");
            if(!backList.contains(i.key()))
            {
                backList<<i.key();
            }
        }
        hash_epc[i.key()] &= 0xfd;//清空扫描标志
        repManager->rfidUpdate(map_epc[i.key()], true);
    }
}

QByteArray EpcManager::storeClearing()
{
    rfidClearing();

    if(storeList.isEmpty())
        return QByteArray();

    cJSON* json = cJSON_CreateObject();
    cJSON* jli = cJSON_CreateArray();

    GoodsList* list;
    Goods* goods;
    QByteArray optName = storeList.at(0)->optName.toLocal8Bit();

    cJSON_AddItemToObject(json, "optName", cJSON_CreateString(optName.data()));

    foreach(list, storeList)
    {
//        cJSON* jGoods = cJSON_CreateArray();
        foreach(goods, list->list_goods)
        {
            cJSON* obj = cJSON_CreateObject();
            cJSON* rlist = cJSON_CreateArray();

            QByteArray packageBarcode = goods->packageBarcode.toLocal8Bit();
            QByteArray chesetCode = list->chesetCode.toLocal8Bit();
            QByteArray barcode = list->barcode.toLocal8Bit();

            cJSON_AddItemToObject(obj, "packageBarcode",cJSON_CreateString(packageBarcode.data()));
            cJSON_AddItemToObject(obj, "chesetCode", cJSON_CreateString(chesetCode.data()));
            cJSON_AddItemToObject(obj, "optType", cJSON_CreateNumber(2));
            cJSON_AddItemToObject(obj, "barcode", cJSON_CreateString(barcode.data()));

            QString rfid;
            if(!goods->storeRfidCodes.isEmpty())
            {
                foreach(rfid, goods->storeRfidCodes)
                {
                    QByteArray rfidCode = rfid.toLocal8Bit();
                    cJSON_AddItemToArray(rlist, cJSON_CreateString(rfidCode.data()));
                }
            }
            cJSON_AddItemToObject(obj, "rfidCodes", rlist);
            cJSON_AddItemToArray(jli, obj);
        }
    }

    cJSON_AddItemToObject(json, "li",jli);

    QByteArray ret = QByteArray(cJSON_Print(json));
    cJSON_Delete(json);

    qDeleteAll(storeList.begin(), storeList.end());
    storeList.clear();

//    qDebug()<<"[storeClearing]"<<ret;
    return ret;
}

QByteArray EpcManager::fetchClearing(QString opt)
{
    qDebug()<<"[dellist]"<<delList;
    if(delList.isEmpty())
        return QByteArray();

    cJSON* json = cJSON_CreateObject();
    cJSON* jli = cJSON_CreateArray();

//    GoodsList* list;
    CabinetManager* cab = CabinetManager::manager();
//    Goods* goods;
    QByteArray optName = opt.toLocal8Bit();

    cJSON_AddItemToObject(json, "optName", cJSON_CreateString(optName.data()));

    foreach(QString epcId, delList)
    {
        cJSON* obj = cJSON_CreateObject();

        QByteArray packageBarcode = epcId.toLocal8Bit();
        QByteArray chesetCode = cab->getCabinetId().toLocal8Bit();
//            QByteArray barcode = list->barcode.toLocal8Bit();

        cJSON_AddItemToObject(obj, "packageBarcode",cJSON_CreateString(packageBarcode.data()));
        cJSON_AddItemToObject(obj, "chesetCode", cJSON_CreateString(chesetCode.data()));
        cJSON_AddItemToObject(obj, "optType", cJSON_CreateNumber(1));
        cJSON_AddItemToObject(obj, "optCount", cJSON_CreateNumber(1));
//            cJSON_AddItemToObject(obj, "barcode", cJSON_CreateString(barcode.data()));

        cJSON_AddItemToArray(jli, obj);
    }

    cJSON_AddItemToObject(json, "li",jli);

    QByteArray ret = QByteArray(cJSON_Print(json));
    cJSON_Delete(json);
//    delList.clear();

//    qDebug()<<"[fetchClearing]"<<ret;
    return ret;
}

QByteArray EpcManager::backClearing(QString opt)
{
    qDebug()<<"[backList]"<<backList;
    if(backList.isEmpty())
        return QByteArray();

    cJSON* json = cJSON_CreateObject();
    cJSON* jli = cJSON_CreateArray();

//    GoodsList* list;
    CabinetManager* cab = CabinetManager::manager();
//    Goods* goods;
    QByteArray optName = opt.toLocal8Bit();

    cJSON_AddItemToObject(json, "optName", cJSON_CreateString(optName.data()));

    foreach(QString epcId, backList)
    {
        cJSON* obj = cJSON_CreateObject();

        QByteArray packageBarcode = epcId.toLocal8Bit();
        QByteArray chesetCode = cab->getCabinetId().toLocal8Bit();
//            QByteArray barcode = list->barcode.toLocal8Bit();

        cJSON_AddItemToObject(obj, "packageBarcode",cJSON_CreateString(packageBarcode.data()));
        cJSON_AddItemToObject(obj, "chesetCode", cJSON_CreateString(chesetCode.data()));
        cJSON_AddItemToObject(obj, "optType", cJSON_CreateNumber(16));
        cJSON_AddItemToObject(obj, "optCount", cJSON_CreateNumber(1));
//            cJSON_AddItemToObject(obj, "barcode", cJSON_CreateString(barcode.data()));

        cJSON_AddItemToArray(jli, obj);
    }

    cJSON_AddItemToObject(json, "li",jli);

    QByteArray ret = QByteArray(cJSON_Print(json));
    cJSON_Delete(json);
//    backList.clear();

//    qDebug()<<"[backClearing]"<<ret;
    return ret;
}

void EpcManager::fetchRst()
{
//    qDebug()<<fetchRst<<delList;
    foreach(QString epc, delList)
    {
        hash_epc[epc] |= 0x04;//标记为被取走
        EpcInfo* info = map_epc[epc];
        info->epcState = hash_epc[epc];
        repManager->rfidUpdate(info, true);
    }
    delList.clear();
}

void EpcManager::backRst()
{
    foreach(QString epc, backList)
    {
        hash_epc[epc] &= 0xfb;//标记为未被取走
        EpcInfo* info = map_epc[epc];
        info->epcState = hash_epc[epc];
        repManager->rfidUpdate(info, true);
    }
    backList.clear();
}

bool EpcManager::insert(QString epc)
{
    if(epc.isEmpty())
        return false;
    if(hash_epc.contains(epc))
    {
        if(hash_epc[epc] & 0x01)//新入物品
        {

        }
        hash_epc[epc] |= 0x02;//标签标记为已被扫描
        qDebug()<<"[epc scaned]"<<epc<<hash_epc[epc];
        return false;
    }
    return true;
//    else
//    {
//        cacheList<<epc;
//        hash_epc.insert(epc, 1);
//        return true;
//    }
}

void EpcManager::append(QStringList l)
{
    cacheList.append(l);
}

void EpcManager::initEpcHash()
{
    activePos = QPoint(0, 0);
    repManager = RepertoryManager::manager();
    QList<EpcInfo*> list_epc = repManager->epcTraversal();
    hash_epc.clear();
    newList.clear();
    qDebug()<<"Init epcHash";
    foreach(EpcInfo* info, list_epc)
    {
        info->epcState &= 0xfd;
        qDebug()<<info->epcId<<info->epcState;
        hash_epc.insert(info->epcId, info->epcState);
        map_epc.insert(info->epcId, info);
    }
}

void EpcManager::clearCache()
{
    cacheList.clear();
}

int EpcManager::epcNum()
{
    return hash_epc.count();
}

void EpcManager::addStoreList(GoodsList *l)
{
    if(l == NULL)
        return;

    foreach(GoodsList* list, storeList)
    {
        if(l->barcode == list->barcode)
            return;
    }

    Goods* goods;
    foreach(goods , l->list_goods)
    {
        appendWatchList(goods->rfidCodes);
        appendRepInfo(goods, QPoint(0,0));
    }
    storeList<<l;
}

QList<GoodsList *> EpcManager::storeLists()
{
    return storeList;
}

void EpcManager::clearStoreLists()
{
    qDeleteAll(storeList.begin(), storeList.end());
    storeList.clear();
}

QStringList EpcManager::checkNew()
{
    return newList;
}

//void EpcManager::checkNewAndDel()
//{
//    qDebug()<<"checkNewAndDel";
//    foreach(QString str, cacheList)
//    {
//        if(hash_epc.contains(str))
//            hash_epc[str] = 1;
//        else
//        {
//            hash_epc.insert(str, 1);
//            newList<<str;
//        }
//    }

//    QHash<QString, int>::const_iterator i;
//    for( i=hash_epc.constBegin(); i!=hash_epc.constEnd(); ++i)
//    {
//        if(i.value() == 0)
//        {
//            delList<<i.key();
//        }
//        else
//        {
//            hash_epc[i.key()] = 0;
//        }
//    }
//}

QStringList EpcManager::checkDel()
{
    QHash<QString, int>::const_iterator i;
    delList.clear();
    newList.clear();

    for( i=hash_epc.constBegin(); i!=hash_epc.constEnd(); ++i)
    {
//        qDebug()<<i.key()<<i.value();
        if(i.value() == 0)
        {
            delList<<i.key();
        }
        else
        {
            hash_epc[i.key()] &= 0xfd;//清空扫描标志
        }
    }
    foreach(QString str, delList)
    {
        hash_epc.remove(str);
    }
    return delList;
}

EpcManager::EpcManager()
{
    clearCache();
    hash_epc.clear();
}

void EpcManager::appendRepInfo(Goods *gInfo, QPoint pos)
{
    foreach(QString rfid , gInfo->rfidCodes)
    {
        EpcInfo* info = new EpcInfo(gInfo, rfid, 0x01, pos);
        repManager->rfidUpdate(info);
    }
}
