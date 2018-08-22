#include "rfidreader.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <QDebug>

RfidReader::RfidReader(QString dev, QObject* parent):QObject(parent)
{
    com = new QextSerialPort(dev);
    com->setBaudRate(BAUD115200);
    com->setDataBits(DATA_8);
    com->setStopBits(STOP_1);
    com->setFlowControl(FLOW_OFF);
    com->setParity(PAR_NONE);
    if(com->open(QIODevice::ReadWrite))
    {
        qDebug()<<dev<<"open success.";
    }

    connect(com, SIGNAL(readyRead()), this, SLOT(readData()));
}

void RfidReader::rfidScan()
{
    QByteArray set_work_ant = QByteArray::fromHex("A00401740000");
    QByteArray real_time_inventory = QByteArray::fromHex("A004018901D1");

    for(int i=0; i<4; i++)
    {
        set_work_ant[4] = i;
        set_work_ant[5] = 231-i;
        waitForWrite(set_work_ant);
        waitForWrite(real_time_inventory);
    }
    writeNext();
}

void RfidReader::scanOneAnt(int antId)
{
    QByteArray set_work_ant = QByteArray::fromHex("A00401740000");
    QByteArray real_time_inventory = QByteArray::fromHex("A004018901D1");

    set_work_ant[4] = antId;
    set_work_ant[5] = 231-antId;
//    waitForWrite(set_work_ant);
    waitForWrite(real_time_inventory);
}

qint64 RfidReader::write(QByteArray data)
{
    qDebug()<<"[RFID write]"<<data.toHex()<<"\n";
    return com->write(data);
}

qint64 RfidReader::writeNext()
{
    if(sendList.isEmpty())
        return -1;

    return write(sendList.takeFirst());
}

void RfidReader::waitForWrite(QByteArray data)
{
    sendList<<QByteArray(data);
}

void RfidReader::readData()
{
//    usleep(100000);
    QByteArray qba = com->readAll();
//    if(qba.at(1) == 0x13)
        qDebug()<<"[readData]"<<qba.toHex()<<"\n";
    writeNext();
}
