#include "devicemanager.h"
#include "config.h"
#include <QDebug>
#include <QByteArray>
#include <unistd.h>

DeviceManager::DeviceManager(QObject *parent) : QObject(parent)
{
#ifndef RUN_IN_ARM
    win_controler = new DeviceSimulator();
    win_controler->show();
#endif
    deviceInit();
}

DeviceManager::~DeviceManager()
{
    delete win_controler;
}

void DeviceManager::deviceInit()
{
    devRfid = new RfidDevice(this);
    connect(devRfid, SIGNAL(rfidIn(QList<rfidChangeInfo*>)), this, SIGNAL(rfidIn(QList<rfidChangeInfo*>)));
    connect(devRfid, SIGNAL(rfidOut(QList<rfidChangeInfo*>)), this, SIGNAL(rfidOut(QList<rfidChangeInfo*>)));
}


QextSerialPort* DeviceManager::comCtrlInit(QString devName, int baudRate, int dataBits, int Parity, int stopBits)
{
    QextSerialPort* device = new QextSerialPort(devName);
    //设置波特率
    device->setBaudRate((BaudRateType)baudRate);
//    qDebug() << (BaudRateType)baudRate;
    //设置数据位
    device->setDataBits((DataBitsType)dataBits);
    //设置校验
    switch(Parity){
    case 0:
        device->setParity(PAR_NONE);
        break;
    case 1:
        device->setParity(PAR_ODD);
        break;
    case 2:
        device->setParity(PAR_EVEN);
        break;
    default:
        device->setParity(PAR_NONE);
        qDebug("set to default : PAR_NONE");
        break;
    }
    //设置停止位
    switch(stopBits){
    case 1:
        device->setStopBits(STOP_1);
        break;
    case 0:
        qDebug() << "linux system can't setStopBits : 1.5!";
        break;
    case 2:
        device->setStopBits(STOP_2);
        break;
    default:
        device->setStopBits(STOP_1);
        qDebug("set to default : STOP_1");
        break;
    }
    //设置数据流控制
    device->setFlowControl(FLOW_OFF);
//    device->setTimeout(5000);

    if(device->open(QIODevice::ReadWrite)){
        qDebug() <<devName<<"open success!";
    }else{
        qWarning()<<devName<< "未能打开串口"<<":该串口设备不存在或已被占用" <<  endl ;
        return NULL;
    }
    return device;
}

void DeviceManager::recvDoorState(bool isopen)
{
    if(!isopen)
        devRfid->startScan();
}


