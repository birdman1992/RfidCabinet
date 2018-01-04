#include "devicesimulator.h"
#include "ui_devicesimulator.h"
#include <QDebug>

DeviceSimulator::DeviceSimulator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceSimulator)
{
    ui->setupUi(this);
    ui->tabWidget->setTabText(0,"读卡器");
    ui->tabWidget->setTabText(1,"RFID");
    ui->tabWidget->setTabText(2,"后台");

    initCardGroup();
    initRfidGroup();
}

DeviceSimulator::~DeviceSimulator()
{
    delete ui;
}



void DeviceSimulator::on_card_submit_clicked()
{
    QByteArray cardId = ui->cardId->text().toLocal8Bit();
    emit cardReaderData(cardId);
    saveInfoToButton(QString(cardId), group_card);
}

void DeviceSimulator::on_rfidIn_clicked()
{
    QByteArray Rfid = ui->rfid->text().toLocal8Bit();
    emit rfidInRange(Rfid);
    saveInfoToButton(QString(Rfid), group_rfid);
}

void DeviceSimulator::on_rfidOut_clicked()
{
    QByteArray Rfid = ui->rfid->text().toLocal8Bit();
    emit rfidInRange(Rfid);
    saveInfoToButton(QString(Rfid), group_rfid);
}

void DeviceSimulator::saveInfoToButton(QString info, QButtonGroup *group)
{
    int i=0;

    for(i=0; i<group->buttons().count(); i++)
    {
        QPushButton* btn = (QPushButton*)group->button(i);
        if(btn->text().isEmpty())
        {
            btn->setText(info);
            return;
        }
    }
}

void DeviceSimulator::initCardGroup()
{
    group_card = new QButtonGroup(this);
    group_card->addButton(ui->card0,0);
    group_card->addButton(ui->card1,1);
    group_card->addButton(ui->card2,2);
    group_card->addButton(ui->card3,3);
    group_card->addButton(ui->card4,4);
}

void DeviceSimulator::initRfidGroup()
{
    group_rfid = new QButtonGroup(this);
    group_rfid->addButton(ui->id_0,0);
    group_rfid->addButton(ui->id_1,1);
    group_rfid->addButton(ui->id_2,2);
    group_rfid->addButton(ui->id_3,3);
    group_rfid->addButton(ui->id_4,4);
    group_rfid->addButton(ui->id_5,5);
    group_rfid->addButton(ui->id_6,6);
    group_rfid->addButton(ui->id_7,7);
    group_rfid->addButton(ui->id_8,8);
    group_rfid->addButton(ui->id_9,9);
    group_rfid->addButton(ui->id_10,10);
    group_rfid->addButton(ui->id_11,11);
    group_rfid->addButton(ui->id_12,12);
    group_rfid->addButton(ui->id_13,13);

}



void DeviceSimulator::on_rfidCheck_clicked()
{
    emit rfidNeedCheck();
}
