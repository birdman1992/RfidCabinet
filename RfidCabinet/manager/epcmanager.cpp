#include "epcmanager.h"
#include <QDebug>

EpcManager* EpcManager::m = new EpcManager;
EpcManager *EpcManager::manager()
{
    return m;
}

bool EpcManager::insert(QString epc)
{
    if(epc.isEmpty())
        return false;
    if(hash_epc.contains(epc))
    {
        hash_epc[epc] = 1;
        return false;
    }
    else
    {
        cacheList<<epc;
        hash_epc.insert(epc, 1);
        return true;
    }
}

void EpcManager::append(QStringList l)
{
    cacheList.append(l);
}

void EpcManager::initEpcHash(QStringList l)
{
    foreach(QString str, l)
    {
        hash_epc.insert(str, 0);
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

    for( i=hash_epc.constBegin(); i!=hash_epc.constEnd(); ++i)
    {
//        qDebug()<<i.key()<<i.value();
        if(i.value() == 0)
        {
            delList<<i.key();
        }
        else
        {
            hash_epc[i.key()] = 0;
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
