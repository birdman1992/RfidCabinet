#include "rfidwidget.h"
#include "ui_rfidwidget.h"
#include <QPainter>
#include <QDebug>
#include <QTableWidget>

RfidWidget::RfidWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RfidWidget)
{
    ui->setupUi(this);
    menu = NULL;

    initMenu();
    setCabinetSize(8,4);
}

RfidWidget::~RfidWidget()
{
    delete ui;
}

void RfidWidget::setCabinetSize(int widNum, int heiNum)
{
    ui->rfidPanel->setColumnCount(widNum);
    ui->rfidPanel->setRowCount(heiNum);

    ui->rfidPanel->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->rfidPanel->verticalHeader()->setResizeMode(QHeaderView::Stretch);

    qDebug()<<ui->rfidPanel->width()<<ui->rfidPanel->height();
    ui->rfidPanel->setSpan(0,0,2,2);
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
}
