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
//    connect(devManager, SIGNAL(rfidIn(int,QByteArray)), serverHttp, SLOT(rfidStore(int,QByteArray)));

    win_layout = new QVBoxLayout(this);
    win_layout->setMargin(0);
    win_layout->setSpacing(0);

    win_stack = new QStackedWidget(this);
    win_layout->addWidget(win_stack);

    win_rfid = new RfidWidget(this);
    win_stack->addWidget(win_rfid);
    connect(win_rfid, SIGNAL(doorStareChanged(bool)), devManager, SLOT(recvDoorState(bool)));

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
