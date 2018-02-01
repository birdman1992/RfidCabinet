#include "cabinet.h"
#include <QHeaderView>

Cabinet::Cabinet(QString _layout, int _colNum, QWidget *parent) :
    QTableWidget(parent)
{
    layout = _layout;
    colNum = _colNum;
    screenPos.setX(-1);
    screenPos.setY(-1);
    this->setShowGrid(false);

    cabSplit(layout);
    initCabinet();
}

Cabinet::~Cabinet()
{
    if(!list_area.isEmpty())
    {
        qDeleteAll(list_area.begin(), list_area.end());
        list_area.clear();
    }
}

void Cabinet::setScreenPos(QPoint pos)
{
    if((pos.x()<0) || (pos.y()<0))
        return;

    screenPos.setX(pos.x());
    screenPos.setY(pos.y());

    if(screenPos.y() >= list_area.count())
        return;

    list_area[screenPos.y()]->setBackColor(QColor(62, 155, 255));
}

void Cabinet::initCabinet()
{
    int j=0;
    for(j=0; j<this->rowCount(); j++)
    {
        RfidArea* area = new RfidArea(QPoint(colNum, j));
        this->setCellWidget(j, 0, area);
        list_area<<area;
    }
}

RfidArea *Cabinet::areaAt(int index)
{
    if(index>=list_area.count())
        return NULL;
    return list_area[index];
}

void Cabinet::resizeEvent(QResizeEvent *)
{
    cabSplit(layout);
}

void Cabinet::cabSplit(QString scale)
{
    if(scale.isEmpty())
    {
        return;
    }
    int rowCount = scale.length();
    int baseCount = getBaseCount(scale);
    int baseHeight = this->geometry().height()/baseCount;
    int i = 0;
    this->setRowCount(rowCount);
    this->setColumnCount(1);

    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setVisible(false);
    this->horizontalHeader()->setVisible(false);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

//    qDebug()<<this->geometry().height()<<baseCount<<baseHeight;
    for(i=0; i<rowCount; i++)
    {
        this->setRowHeight(i,baseHeight*(scale.mid(i,1).toInt()));
    }
}

int Cabinet::getBaseCount(QString scale)
{
    int i = 0;
    int ret = 0;
    if(scale.isEmpty())
        return ret;

    for(i=0; i<scale.length(); i++)
    {
        ret += scale.mid(i,1).toInt();
    }
    return ret;
}
