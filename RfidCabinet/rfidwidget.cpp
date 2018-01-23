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
    setCabinetSize(1,1);
    readCellsInfo();
}

RfidWidget::~RfidWidget()
{
    delete ui;
}

//初始化表格行列
void RfidWidget::setCabinetSize(int widNum, int heiNum)
{
    ui->rfidPanel->setColumnCount(widNum);
    ui->rfidPanel->setRowCount(heiNum);

    ui->rfidPanel->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->rfidPanel->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    qDebug()<<ui->rfidPanel->width()<<ui->rfidPanel->height();
//    ui->rfidPanel->setSpan(0,0,2,2);
//    RfidArea* rfidCase = new RfidArea(ui->rfidPanel);
    //    ui->rfidPanel->setCellWidget(0,0,rfidCase);
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
    setMenuPow(0);
}

void RfidWidget::setMenuPow(int _pow)
{
    ui->fetch->hide();
    ui->store->hide();
    ui->cell_config->hide();
    switch(_pow){
    case 0:
        break;
    default:
        break;
    }
}

void RfidWidget::creatRfidCells()
{
    int i=0;//列
    int j=0;//行

    RfidArea* rfidCell;

    for(i=0; i<ui->rfidPanel->columnCount();i++)//初始化列
    {
        for(j=0; j<ui->rfidPanel->rowCount(); j++)
        {
            if(!pointIsInSpan(j, i))
            {
                rfidCell = new RfidArea(ui->rfidPanel);
                ui->rfidPanel->setCellWidget(j,i,rfidCell);
                listCells<<rfidCell;
                antsMap.insert(listCells.count(), rfidCell);
                rfidCell->setAntId(listCells.count());
                rfidCell->updateInfo();
//                qDebug()<<"[creat cell]"<<j<<i;
            }
//            j+=ui->rfidPanel->rowSpan(j,i);
        }
    }
    qDebug()<<listCells.count();

    ui->rfidPanel->setShowGrid(false);
    onSpan = false;

    menuLock();
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
    ui->rfidPanel->clear();
    ui->rfidPanel->setRowCount(1);
    ui->rfidPanel->setColumnCount(1);
    ui->rfidPanel->setShowGrid(true);
    colCount = 1;
    rowCount = 1;
}

void RfidWidget::menuLock()
{
    ui->col_add->setEnabled(false);
    ui->col_red->setEnabled(false);
    ui->row_add->setEnabled(false);
    ui->row_red->setEnabled(false);
    ui->span->setEnabled(false);
    ui->span_cancel->setEnabled(false);
}

void RfidWidget::menuUnlock()
{
    ui->col_add->setEnabled(true);
    ui->col_red->setEnabled(true);
    ui->row_add->setEnabled(true);
    ui->row_red->setEnabled(true);
    ui->span->setEnabled(true);
    ui->span_cancel->setEnabled(true);
}

void RfidWidget::setCellsLayout(QByteArray cellsLayout)
{
    if((cellsLayout.length() - 2)%4)
    {
        qDebug()<<"[setCellsLayout]"<<"error layout.";
    }
    ui->rfidPanel->setColumnCount(cellsLayout.at(0));
    ui->rfidPanel->setRowCount(cellsLayout.at(1));
    qDebug()<<ui->rfidPanel->rowCount()<<ui->rfidPanel->columnCount();

    int i=0;
    for(i=2; i<cellsLayout.length(); i+=4)
    {
        int span_x = cellsLayout.at(i);
        int span_y = cellsLayout.at(i+1);
        int span_w = cellsLayout.at(i+2);
        int span_h = cellsLayout.at(i+3);
//        qDebug()<<"[span]"<<cellsLayout.at(i)<<cellsLayout.at(i+1)<<cellsLayout.at(i+2)<<cellsLayout.at(i+3);
        ui->rfidPanel->setSpan(span_y, span_x, span_h, span_w);
        listSpans<<new QRectF(span_x, span_y, span_w, span_h);
    }
    creatRfidCells();
}

QByteArray RfidWidget::getCellsLayout()
{
    int i=0;
    QByteArray ret = QByteArray();

    ret.append(ui->rfidPanel->columnCount());
    ret.append(ui->rfidPanel->rowCount());

    for(i=0; i<listSpans.count(); i++)
    {
        ret.append(listSpans.at(i)->left());
        ret.append(listSpans.at(i)->top());
        ret.append(listSpans.at(i)->width());
        ret.append(listSpans.at(i)->height());
    }
    return ret;
}

void RfidWidget::saveCellsInfo()
{
    QFile fLayout(FILE_CONFIG_CABINET_LAYOUT);
    fLayout.open(QFile::WriteOnly);
    QByteArray cellsInfo = getCellsLayout();
    qDebug()<<"[saveCellsInfo]"<<cellsInfo.toHex();
    fLayout.write(cellsInfo.toHex());
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
    setCellsLayout(cellsInfo);
}

void RfidWidget::on_row_add_clicked()
{
    ui->rfidPanel->setRowCount(++rowCount);
}

void RfidWidget::on_row_red_clicked()
{
    ui->rfidPanel->setRowCount(--rowCount);
}

void RfidWidget::on_col_add_clicked()
{
    ui->rfidPanel->setColumnCount(++colCount);
}

void RfidWidget::on_col_red_clicked()
{
    ui->rfidPanel->setColumnCount(--colCount);
}

void RfidWidget::on_save_clicked()
{
    creatRfidCells();
    saveCellsInfo();
}

void RfidWidget::on_span_toggled(bool checked)
{
    onSpan = checked;
}

void RfidWidget::on_rfidPanel_clicked(const QModelIndex &index)
{
    if(!onSpan)
        return;

    if((spanX == -1) || (spanY == -1))
    {
        spanX = index.column();
        spanY = index.row();
        return;
    }

    int spanColNum = index.column()-spanX+1;
    int spanRowNum = index.row()-spanY+1;

    if((spanColNum<2) && (spanRowNum<2))
    {
        spanX = -1;
        spanY = -1;
        return;
    }

//    qDebug()<<"[span]"<<spanX<<spanY<<spanColNum<<spanRowNum;
    ui->rfidPanel->setSpan(spanY, spanX, spanRowNum, spanColNum);
    listSpans<<new QRectF(spanX, spanY, spanColNum, spanRowNum);
    spanX = -1;
    spanY = -1;
}

void RfidWidget::on_span_cancel_clicked()
{
    ui->span->setChecked(false);
    ui->rfidPanel->clearSpans();
    qDeleteAll(listSpans.begin(), listSpans.end());
    listSpans.clear();
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
    rfidCellClear();
    if(!listCells.isEmpty())
    {
        qDeleteAll(listCells.begin(), listCells.end());
        listCells.clear();
    }
    if(!listSpans.isEmpty())
    {
        qDeleteAll(listSpans.begin(), listSpans.end());
        listSpans.clear();
    }
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
