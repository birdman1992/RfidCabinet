#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    initServer();

    devManager = new DeviceManager(this);
    win_layout = new QVBoxLayout(this);
    win_layout->setMargin(0);
    win_layout->setSpacing(0);

    win_stack = new QStackedWidget(this);
    win_layout->addWidget(win_stack);

    win_rfid = new RfidWidget(this);
    win_stack->addWidget(win_rfid);
    connect(win_rfid, SIGNAL(doorStareChanged(bool)), devManager, SLOT(recvDoorState(bool)));
    connect(win_rfid, SIGNAL(rfidStoreReq(QList<rfidChangeInfo*>)), serverHttp, SLOT(rfidStore(QList<rfidChangeInfo*>)));
    connect(win_rfid, SIGNAL(rfidFetchReq(QList<rfidChangeInfo*>)), serverHttp, SLOT(rfidFetch(QList<rfidChangeInfo*>)));
    connect(devManager, SIGNAL(rfidIn(QList<rfidChangeInfo*>)), win_rfid, SLOT(rfidIn(QList<rfidChangeInfo*>)));
    connect(devManager,SIGNAL(rfidOut(QList<rfidChangeInfo*>)), win_rfid, SLOT(rfidOut(QList<rfidChangeInfo *>)));
    connect(serverHttp, SIGNAL(newStoreList(QList<GoodsInfo*>)), win_rfid, SLOT(goodsIn(QList<GoodsInfo*>)));
    connect(serverHttp, SIGNAL(newFetchList(QList<GoodsInfo*>)), win_rfid, SLOT(goodsOut(QList<GoodsInfo*>)));

//    win_stack->setCurrentIndex(0);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::initServer()
{
    serverHttp = new HttpApi(this);
}
