#include "rfidwidget.h"
#include "ui_rfidwidget.h"
#include <QPainter>
#include <QDebug>
#include <QTableWidget>
#include <QMessageBox>
#include <QHeaderView>

#include "config.h"

RfidWidget::RfidWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RfidWidget)
{
    ui->setupUi(this);
    menu = NULL;
    rowCount = 1;
    colCount = 1;
    spanX = -1;
    spanY = -1;
    repManager = new RepertoryManager(this);

    initMenu();
    initCabType(QString(CAB_TYPE).split("#"));
    readCellsInfo();
}

RfidWidget::~RfidWidget()
{
    delete ui;
}

void RfidWidget::rfidIn(QList<GoodsInfo *> listStore)
{
    GoodsInfo* info;
    foreach(info, listStore)
    {
        RfidArea* cell = antsMap.value(info->antId,NULL);
        if(cell == NULL)
            return;

        repManager->rfidIn(info);
        cell->updateInfo();
    }
}

void RfidWidget::rfidOut(int antId, QByteArray rfid)
{

}

void RfidWidget::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void RfidWidget::initMenu()
{
    if(menu != NULL)
        return;

    menu = new QButtonGroup(this);
    menu->addButton(ui->fetch);
    menu->addButton(ui->store);

    menu->setExclusive(true);
    ui->menuStack->setCurrentIndex(0);
    setMenuPow(1);
}

void RfidWidget::initCabType(QStringList typeList)
{
    ui->cabType->addItems(typeList);
    ui->cabType->setCurrentIndex(0);
}

void RfidWidget::setMenuPow(int _pow)
{
    ui->fetch->hide();
    ui->store->hide();
    ui->cell_config->hide();
    switch(_pow){
    case 0:
        break;
    case 1:
        ui->cell_config->show();
    default:
        break;
    }
}

void RfidWidget::creatRfidCells()
{

}

bool RfidWidget::pointIsInSpan(int row, int col)
{
    int i=0;

    for(i=0; i<listSpans.count(); i++)
    {
        QPointF X = QPointF(col,row);
        QPointF A = listSpans.at(i)->topLeft();
//        QPointF B = listSpans.at(i)->topRight();
//        QPointF C = listSpans.at(i)->bottomLeft();
//        QPointF D = listSpans.at(i)->bottomRight();

//        qDebug()<<A<<X;
        QRectF rectX = QRectF(X, QSizeF(1,1));

        if(X==A)
            return false;

        if(listSpans.at(i)->contains(rectX))
        {
//            qDebug()<<rectX;
            return true;
        }
    }
    return false;
}

void RfidWidget::rfidCellClear()
{
    colCount = 1;
    rowCount = 1;
}

void RfidWidget::menuLock()
{

}

void RfidWidget::menuUnlock()
{

}

void RfidWidget::saveCellsInfo()
{
    QFile fLayout(FILE_CONFIG_CABINET_LAYOUT);
    fLayout.open(QFile::WriteOnly);
//    QByteArray cellsInfo = getCellsLayout();
//    qDebug()<<"[saveCellsInfo]"<<cellsInfo.toHex();
//    fLayout.write(cellsInfo.toHex());
    fLayout.close();
}

void RfidWidget::readCellsInfo()
{
    QFile fLayout(FILE_CONFIG_CABINET_LAYOUT);
    if(!fLayout.exists())
        return;
    fLayout.open(QFile::ReadOnly);
    QByteArray cellsInfo = QByteArray::fromHex(fLayout.readAll());
    qDebug()<<"[readCellsInfo]"<<cellsInfo.toHex();
//    setCellsLayout(cellsInfo);
}

void RfidWidget::on_save_clicked()
{
    creatRfidCells();
    saveCellsInfo();
}

void RfidWidget::on_rfidPanel_clicked(const QModelIndex &index)
{

}

void RfidWidget::on_back_clicked()
{
    ui->menuStack->setCurrentIndex(0);
}

void RfidWidget::on_cell_config_clicked()
{
    ui->menuStack->setCurrentIndex(1);
}

void RfidWidget::on_clear_config_clicked()
{
    menuUnlock();
}

void RfidWidget::on_test_open_clicked(bool checked)
{
    if(checked)//开门
    {
        ui->test_open->setText("关门");
    }
    else
    {
        ui->test_open->setText("开门模拟");
    }
    emit doorStareChanged(checked);
}

void RfidWidget::on_cabType_currentTextChanged(const QString &arg1)
{
    cabSplit(arg1, ui->preCab);
}

void RfidWidget::cabSplit(QString scale, QTableWidget *table)
{
    if(scale.isEmpty()||(table == NULL))
    {
        return;
    }
    int rowCount = scale.length();
    int baseCount = getBaseCount(scale);
    int baseHeight = table->geometry().height()/baseCount;
    int i = 0;
    table->setRowCount(rowCount);
    table->setColumnCount(1);

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setVisible(false);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    qDebug()<<table->geometry().height()<<baseCount<<baseHeight;
    for(i=0; i<rowCount; i++)
    {
        table->setRowHeight(i,baseHeight*(scale.mid(i,1).toInt()));
    }
}

int RfidWidget::getBaseCount(QString scale)
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
