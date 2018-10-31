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
    fluctNum = -1;
    manager_epc = EpcManager::manager();
    manager_cab = CabinetManager::manager();
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

    getOutputPower(readerId);
    setOutputPower(readerId, (char)manager_cab->getAntPow());
    setAndSaveImpinjFastTid(readerId, true);
}

void RfidReader::rfidScan()
{
    QByteArray set_work_ant = QByteArray::fromHex("A00401740000");
    QByteArray real_time_inventory = QByteArray::fromHex("A004018901D1");
    scanTime.start();
    curAnt = -1;
//    setScanAnt(1, 1);
//    getRealTimeInventory(1, 5);
    for(int i=0; i<4; i++)
    {
        setScanAnt(readerId, i);
        getRealTimeInventory(readerId, 2);
    }
    //    getInventoryBuffer(1);
}

QList<rfidChangeInfo*> RfidReader::getScanAddList()
{
    QList<rfidChangeInfo*> ret = listAdd;
    listAdd.clear();
    return ret;
}

QList<rfidChangeInfo*> RfidReader::getScanDelList()
{
    QList<rfidChangeInfo*> ret = listDel;
    listDel.clear();
    return ret;
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

void RfidReader::scanFinish()
{
    qDebug()<<"[scanFinish]";
    qDebug()<<"[use time]"<<scanTime.elapsed()/1000.0<<"s";
    QStringList l = manager_epc->checkDel();
    qDebug()<<"[EPC]"<<"total:"<<manager_epc->epcNum()<<"add:"<<listAdd.count()<<"remove:"<<l.count();
    if(listAdd.count() | l.count())
        fluctNum++;
    qDebug()<<"[fluctNum]"<<fluctNum;
    qDebug()<<"+"<<printList(listAdd);
    qDebug()<<"-"<<l;

    foreach(QString str, l)
    {
        listDel<<new rfidChangeInfo(readerId, QByteArray::fromHex(str.toLocal8Bit()));
    }
    emit scanFinished();
    rfidScan();
}

void RfidReader::getOutputPower(char address)
{
    qDebug()<<"getoutputpower>>>>>>>>";
    QByteArray qba = rfidCmd(address, 0x77, QByteArray());
//    waitFlag = false;
    waitForWrite(qba);
}

void RfidReader::setOutputPower(char address, char pow)
{
    qDebug()<<"[setOutputPower]"<<(int)pow;
    QByteArray qba = rfidCmd(address, 0x76, pow);
//    waitFlag = false;
    waitForWrite(qba);
}

void RfidReader::setAndSaveImpinjFastTid(char address, bool useTid)
{
    char ctrlWd = 0;
    if(useTid)
    {
        ctrlWd = 0x8D;
    }
    QByteArray qba = rfidCmd(address, 0x8D, ctrlWd);
//    waitFlag = false;
    waitForWrite(qba);
}

void RfidReader::parPackage(QByteArray qba)
{
    char cmd = qba[3];
    switch(cmd){
    case 0x89://实时标签扫描
        parEpc(qba);break;
    case 0x76://
        parSetOutputPower(qba);break;
    case 0x77://
        parGetOutputPower(qba);break;
    case 0x8D:
        if(qba[4] == (char)0x10) qDebug()<<"[set use tid]:success";break;
    default:
        break;
    }
}

QString RfidReader::parEpc(QByteArray qba)
{
    int len = qba[1];
    int antId = 0;
    if(len == 4)//操作失败
    {
        nextFlag = true;
        if(qba.at(4) == 0x22)//天线未连接
        {
//            qWarning()<<"[RfidReader]"<<"ant"<<curAnt+1<<"not connected.";
            if((curAnt+1) == 3)
            {
                scanFinish();
            }
        }
        return QString();
    }
    if(len == 0x0a)
    {
        nextFlag = true;
        antId = qba[4];
//        qDebug()<<antId;
        if(antId == 3)
        {
            scanFinish();
        }
        return QString();
    }

    antId = qba[4]&0x03;
    int epcLen = qba.size()-9;
    char rssi = qba.at(qba.size()-2);
    QString epc = qba.mid(7, epcLen).toHex();
    nextFlag = false;
    curAnt = antId;

    if(epcFilter(epc))
        return QString();

//    qDebug()<<"[ant:"<<antId<<"]"<<epc<<QByteArray(1, rssi).toHex();
//    qDebug()<<qba.toHex();
    if(manager_epc->insert(epc))
    {
        listAdd<<new rfidChangeInfo(readerId, QByteArray::fromHex(epc.toLocal8Bit()));
    }

    return epc;
}

void RfidReader::parGetOutputPower(QByteArray qba)
{
    QString showStr;
    if(qba.size() == 6)
    {
        showStr = QString("[output power]\nant0:%1/33 dBm\nant1:%2/33 dBm\nant2:%3/33 dBm\nant3:%4/33 dBm\n").arg((int)qba[4]).arg((int)qba[4]).arg((int)qba[4]).arg((int)qba[4]);
    }
    else if(qba.size() == 9)
    {
        showStr = QString("[output power]\nant0:%1/33 dBm\nant1:%2/33 dBm\nant2:%3/33 dBm\nant3:%4/33 dBm\n").arg((int)qba[4]).arg((int)qba[5]).arg((int)qba[6]).arg((int)qba[7]);
    }
    qDebug()<<qPrintable(showStr);
}

void RfidReader::parSetOutputPower(QByteArray qba)
{
    if(qba[4] = 0x10)
    {
        qDebug()<<"[set output power]:success";
    }
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

void RfidReader::setReaderAddress(char address)
{
     QByteArray qba = rfidCmd(1, 0x73, address);
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
//    qDebug()<<"[RFID write]"<<data.toHex()<<"\n";
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

QStringList RfidReader::printList(QList<rfidChangeInfo *> l)
{
    QStringList ret;
    foreach(rfidChangeInfo* info, l)
    {
        ret<<QString(info->rfid.toHex());
    }
    return ret;
}

bool RfidReader::epcFilter(QString epc)
{
    QStringList filter;
    filter<<"22faff45603d01740470e45f";//<<"22faff4560f0014000000000"<<"22faff4561b5014000000000"<<"22faff4561c1014000000000"<<"22faff45606f014000000000";
    if(filter.indexOf(epc) == -1)
        return false;
    else
        return true;
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
//    qDebug()<<"[readData]"<<qba.toHex()<<"\n";
    readDataCache();
    if(nextFlag)
        writeNext();
}
