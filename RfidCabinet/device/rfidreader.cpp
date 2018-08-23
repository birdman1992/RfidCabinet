#include "rfidreader.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <QDebug>

RfidReader::RfidReader(int address, QString dev, QObject* parent):QObject(parent)
{
    waitFlag = false;
    readerId = address;
    curAnt = -1;
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
    curAnt = -1;
//    setScanAnt(1, 1);
//    getRealTimeInventory(1, 5);
    for(int i=0; i<4; i++)
    {
        setScanAnt(readerId, i);
        getRealTimeInventory(readerId, 5);
    }
//    getInventoryBuffer(1);
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

void RfidReader::readDataCache()
{
    if(dataCache.isEmpty())
        return;

    while((dataCache.at(0) != 0xa0))
    {
        dataCache.remove(0,1);
        if(dataCache.isEmpty())
            return;
    }

    int packageLen = dataCache[1]+2;
    if(dataCache.size()<packageLen)//包不完整
        return;

    QByteArray package = dataCache.left(packageLen);
    dataCache.remove(0,packageLen);

    parPackage(package);
    readDataCache();
}

void RfidReader::parPackage(QByteArray qba)
{
    char cmd = qba[3];
    switch(cmd){
    case 0x89://实时标签扫描
        parEpc(qba);break;
    default:
        break;
    }
}

QString RfidReader::parEpc(QByteArray qba)
{
    int len = qba[1];
    if(len == 4)//操作失败
    {
        nextFlag = true;
        if(qba.at(4) == 0x22)//天线未连接
        {
            qWarning()<<"[RfidReader]"<<"ant"<<curAnt+1<<"not connected.";
        }
        return QString();
    }
    if(len == 0x0a)
    {
        nextFlag = true;
        return QString();
    }

    int antId = qba[4]&0x03;
    int epcLen = qba.size()-9;
    QString epc = qba.mid(7, epcLen).toHex();
    nextFlag = false;
    curAnt = antId;
    qDebug()<<"[ant:"<<antId<<"]"<<epc;
    return epc;
}

void RfidReader::setScanAnt(char address, char antId)
{
    QByteArray qba = rfidCmd(address, 0x74, antId);
    nextFlag = true;
    waitForWrite(qba);
}

void RfidReader::rfidInventory(char address, char repeat)
{
    QByteArray qba = rfidCmd(address, 0x80, repeat);
    waitForWrite(qba);
}

void RfidReader::getInventoryBuffer(char address)
{
    QByteArray qba = rfidCmd(address, 0x90, QByteArray());
    waitForWrite(qba);
}

void RfidReader::getRealTimeInventory(char address, char repeat)
{
    QByteArray qba = rfidCmd(address, 0x89, repeat);
    waitForWrite(qba);
}

QByteArray RfidReader::rfidCmd(char address, char cmd, QByteArray data)
{
    QByteArray ret = QByteArray::fromHex("a000000000");
    ret.insert(3, data);
    ret[1] = ret.size()-2;
    ret[2] = address;
    ret[3] = cmd;
    ret[ret.size()-1] = checkSum(ret);
    return ret;
}

QByteArray RfidReader::rfidCmd(char address, char cmd, char data)
{
    QByteArray ret = QByteArray::fromHex("a00000000000");
    ret[1] = ret.size()-2;
    ret[2] = address;
    ret[3] = cmd;
    ret[4] = data;
    ret[ret.size()-1] = checkSum(ret);
    return ret;
}

char RfidReader::checkSum(QByteArray qba)
{
    unsigned char i,uSum=0;
    unsigned char uBuffLen = qba.size();
    unsigned char* uBuff = (unsigned char*)qba.data();

    for(i=0;i<uBuffLen;i++)
    {
        uSum = uSum + uBuff[i];
    }
    uSum = (~uSum) + 1;
    return uSum;
}

qint64 RfidReader::write(QByteArray data)
{
    qDebug()<<"[RFID write]"<<data.toHex()<<"\n";
    return com->write(data);
}

qint64 RfidReader::writeNext()
{
    if(sendList.isEmpty())
    {
        waitFlag = false;
        return -1;
    }
    waitFlag = true;

    return write(sendList.takeFirst());
}

void RfidReader::waitForWrite(QByteArray data)
{
    sendList<<QByteArray(data);
    if(!waitFlag)
        writeNext();
}

void RfidReader::readData()
{
//    usleep(100000);
    QByteArray qba = com->readAll();
    dataCache.append(qba);
//    if(qba.at(1) == 0x13)
    qDebug()<<"[readData]"<<qba.toHex()<<"\n";
    readDataCache();
    if(nextFlag)
        writeNext();
}
