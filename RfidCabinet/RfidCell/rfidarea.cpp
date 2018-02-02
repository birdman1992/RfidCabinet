#include "rfidarea.h"
#include "ui_rfidarea.h"
#include <qgraphicseffect.h>
#include <QPainter>
#include <QString>
#include <QDebug>
#include "config.h"

RfidArea::RfidArea(QPoint pos, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RfidArea)
{
    ui->setupUi(this);
    QGraphicsDropShadowEffect *shadow_effect = new QGraphicsDropShadowEffect(this);
    shadow_effect->setOffset(0,0);
    shadow_effect->setColor(Qt::black);
    shadow_effect->setBlurRadius(8);
    areaPos.setX(pos.x());
    areaPos.setY(pos.y());
    font = new QFont("微软雅黑");
    font->setPixelSize(15);
    this->setFont(*font);
    ui->infoLab->setGraphicsEffect(shadow_effect);
}

RfidArea::~RfidArea()
{
    delete ui;
}

QPoint RfidArea::getPos()
{
    return areaPos;
}

QString RfidArea::getRepFile()
{
    return QString(FILE_CONFIG_REP) + QString("%1#%2.rep").arg(areaPos.x()).arg(areaPos.y());
}

void RfidArea::setAntId(int id)
{
    antId = id;
}

QStringList RfidArea::addId(QStringList list)
{
    QStringList ret = diffList(list, list_id);
    list_id<<ret;
    return ret;
}

QStringList RfidArea::removeId(QStringList list)
{
    QStringList ret = sameList(list, list_id);
    QString str;

    foreach(str, ret)
    {
        list_id.removeOne(str);
    }

    return ret;
}

bool RfidArea::addId(QString _id)
{
    if(list_id.indexOf(_id) != -1)
    {
        return false;
    }
    list_id<<_id;
    return true;
}

bool RfidArea::removeId(QString _id)
{
    int index = list_id.indexOf(_id);
    if(index == -1)
        return false;
    list_id.removeAt(index);
    return true;
}

//找出newList中不被包含在fixList中的项
QStringList RfidArea::diffList(QStringList newList, QStringList fixList)
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
    if(!tar.isEmpty())
    {
        qDebug()<<"[diffList]"<<ret.count()<<"/"<<newList.count();
        qDebug()<<tar;
    }

    return ret;
}

//找出newList中被包含在fixList中的项
QStringList RfidArea::sameList(QStringList newList, QStringList fixList)
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

void RfidArea::setBackColor(QColor c)
{
    QString ret = QString("color:rgb(255,255,255);background-color: rgb(%1, %2, %3);").arg(c.red()).arg(c.green()).arg(c.blue());
    ui->infoLab->setStyleSheet(ret);
}

void RfidArea::updateInfo()
{
//    qDebug()<<"[updateInfo]"<<areaPos;
    ui->infoLab->setText(getShowStr());
}

void RfidArea::readAreaData()
{
    QSettings settings(getRepFile(), QSettings::IniFormat);
    QStringList rfidList = settings.childGroups();
    if(rfidList.isEmpty())
        return;

    QString goodsId;
    foreach(goodsId, rfidList)
    {
        settings.beginGroup(goodsId);
        QString str = settings.value("rfids", QString()).toString();
        addId(str.split(" ", QString::SkipEmptyParts));
        settings.endGroup();
    }
//    qDebug()<<"[readAreaData]"<<list_id;
}

QString RfidArea::getShowStr()
{
    QSettings settings(getRepFile(), QSettings::IniFormat);
    QStringList rfidList = settings.childGroups();
    if(rfidList.isEmpty())
        return QString();

    QString showStr = QString();
    QString goodsId;
    foreach(goodsId, rfidList)
    {
        settings.beginGroup(goodsId);
        showStr+=QString("%1×%2\n").arg(settings.value("name").toString()).arg(settings.value("num").toString());
        settings.endGroup();
    }
//    qDebug()<<"[getShowStr]"<<showStr;
    return showStr;
}

void RfidArea::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
