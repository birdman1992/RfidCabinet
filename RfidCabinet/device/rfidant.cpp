#include "rfidant.h"
#include <QDebug>

RfidAnt::RfidAnt(int _antId, RfidArea *_area, QObject *parent) :
    QObject(parent)
{
    antId = _antId;
    area = _area;
}

QStringList RfidAnt::addId(QStringList& list)
{
    QStringList ret = diffList(list, area->list_id);
    area->list_id<<ret;
    return ret;
}

QStringList RfidAnt::removeId(QStringList list)
{
    QStringList ret = sameList(list, area->list_id);
    QString str;

    foreach(str, ret)
    {
        area->list_id.removeOne(str);
    }

    return ret;
}

bool RfidAnt::addId(QString _id)
{
    if(area->list_id.indexOf(_id) != -1)
    {
        return false;
    }
    area->list_id<<_id;
    return true;
}

bool RfidAnt::removeId(QString _id)
{
    int index = area->list_id.indexOf(_id);
    if(index == -1)
        return false;
    area->list_id.removeAt(index);
    return true;
}

int RfidAnt::getAntId()
{
    return antId;
}

QPoint RfidAnt::getPos()
{
    if(area == NULL)
        return QPoint(-1,-1);
    return area->getPos();
}

//找出newList中不被包含在fixList中的项
QStringList RfidAnt::diffList(QStringList newList, QStringList fixList)
{
    QStringList ret;
    QStringList tar;
    QString str;

    foreach(str, newList)
    {
        if(fixList.indexOf(str) == -1)
        {
            ret<<str;
        }
        else
        {
            tar<<str;
        }
    }
    if(tar.isEmpty())
    {
        qDebug()<<"[diffList]"<<ret.count()<<"/"<<newList.count();
        qDebug()<<tar;
    }

    return ret;
}

//找出newList中被包含在fixList中的项
QStringList RfidAnt::sameList(QStringList newList, QStringList fixList)
{
    QStringList ret;
    QStringList tar;
    QString str;

    foreach(str, newList)
    {
        if(fixList.indexOf(str) >= 0)
        {
            ret<<str;
        }
        else
        {
            tar<<str;
        }
    }
    if(!tar.isEmpty())
    {
        qDebug()<<"[sameList]"<<ret.count()<<"/"<<newList.count();
        qDebug()<<tar;
    }

    return ret;
}
