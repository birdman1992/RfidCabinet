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

void RfidArea::setBackColor(QColor c)
{
    QString ret = QString("color:rgb(255,255,255);background-color: rgb(%1, %2, %3);").arg(c.red()).arg(c.green()).arg(c.blue());
    ui->infoLab->setStyleSheet(ret);
}

void RfidArea::updateInfo()
{
    qDebug()<<"[updateInfo]"<<areaPos;
    ui->infoLab->setText(getShowStr());
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
    }
    qDebug()<<"[getShowStr]"<<showStr;
    return showStr;
}

void RfidArea::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
