#include "devicemanager.h"
#include "config.h"
#include <QDebug>
#include <QByteArray>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <linux/hidraw.h>
#include <linux/hiddev.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>

char dev_path[2][24] = {0};

DeviceManager::DeviceManager(QObject *parent) : QObject(parent)
{
#ifndef RUN_IN_ARM
    win_controler = new DeviceSimulator();
    win_controler->show();
    connect(win_controler, SIGNAL(rfidIn(QList<rfidChangeInfo*>)), this, SIGNAL(rfidIn(QList<rfidChangeInfo*>)));
    connect(win_controler, SIGNAL(rfidOut(QList<rfidChangeInfo*>)), this, SIGNAL(rfidOut(QList<rfidChangeInfo*>)));
#endif
    deviceInit();
}

DeviceManager::~DeviceManager()
{
#ifndef RUN_IN_ARM
    delete win_controler;
#endif
    delete devRfid;
}

void DeviceManager::deviceInit()
{
    qDebug("deviceInit");

    devRfid = new RfidDevice(this);
    connect(devRfid, SIGNAL(rfidIn(QList<rfidChangeInfo*>)), this, SIGNAL(rfidIn(QList<rfidChangeInfo*>)));
    connect(devRfid, SIGNAL(rfidOut(QList<rfidChangeInfo*>)), this, SIGNAL(rfidOut(QList<rfidChangeInfo*>)));

    //初始化锁控:波特率,数据位,奇偶校验,停止位
//    com_lock_ctrler = new QSerialPort(DEV_LOCK_CTRL);
//    com_lock_ctrler->com_init(38400,0,8,'N',1);
    get_path();
    qDebug()<<"[get dev path1]"<<dev_path[0];
    qDebug()<<"[get dev path2]"<<dev_path[1];
    //控制串口初始化
    com_lock_ctrl = comCtrlInit(DEV_LOCK_CTRL, 38400, 8, 0, 1);
    connect(com_lock_ctrl, SIGNAL(readyRead()), this, SLOT(readLockCtrlData()));
//    int ret = com_lock_ctrl->write(QByteArray("test"));
//    qDebug()<<"[write to lock]"<<DEV_LOCK_CTRL<<ret<<QByteArray("test");

    //初始化读卡器
    hid_card_reader = new QHid(this);
    hid_card_reader->hidOpen(dev_path[0]);
    connect(hid_card_reader, SIGNAL(hidRead(QByteArray)), this, SLOT(readCardReaderData(QByteArray)));

    //初始化扫码设备
    hid_code_scan = new QHid(this);
    hid_code_scan->hidOpen(dev_path[1]);
    connect(hid_code_scan, SIGNAL(hidRead(QByteArray)), this, SLOT(readCodeScanData(QByteArray)));

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

void DeviceManager::insertRfid(QStringList ids)
{/*qDebug()<<"[insertRfid]"<<ids;*/
    devRfid->insertRfid(ids);
}

void DeviceManager::test()
{
    qDebug("test>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}

void DeviceManager::readLockCtrlData()
{
    ::usleep(20000);
    QByteArray qba = com_lock_ctrl->readAll();
    qDebug()<<"[readLockCtrlData]"<<qba.toHex();
}

void DeviceManager::readCardReaderData(QByteArray qba)
{
    QString upStr = QString(qba);
    qba = upStr.toUpper().toLocal8Bit();
    qDebug()<<"[readCardReaderData]"<<qba;
    emit cardReaderData(QString(qba));
    devRfid->startScan();
}

void DeviceManager::readCodeScanData(QByteArray qba)
{
    qDebug()<<"[readCodeScanData]"<<qba;
    emit codeScanData(qba);
}

int DeviceManager::get_dev_info(char *dev_name,USBINFO* uInfo)
{
    int fd;
    int res;
    char buf[256];
    struct hidraw_report_descriptor rpt_desc;
    struct hidraw_devinfo info;

    /* Open the Device with non-blocking reads. In real life,
     don't use a hard coded path; use libudev instead. */
    fd = open(dev_name, O_RDWR|O_NONBLOCK);
    if (fd < 0)
    {
        perror("Unable to open device");
        return 1;
    }
    memset(&rpt_desc, 0x0, sizeof(rpt_desc));
    memset(&info, 0x0, sizeof(info));
    memset(buf, 0x0, sizeof(buf));

    // Get Raw Info
    res = ioctl(fd, HIDIOCGRAWINFO, &info);
    if (res < 0)
        perror("HIDIOCGRAWINFO");
    else
    {
        uInfo->vid = info.vendor;
        uInfo->pid = info.product;
    }
    close(fd);
    return 0;

}

int DeviceManager::get_path(void)
{
    int event = 0;
    int count = 0;
    char path[24] = {"/dev/"};
    DIR* pDir = NULL;
    USBINFO* usb_info = (USBINFO *)malloc(sizeof(USBINFO));
    struct dirent *pFile = NULL;
    if ((pDir=opendir("/dev/")) == NULL)
    {
        printf("Update: Open update directory error!");
        return 0;
    }
    else
    {
        while((pFile = readdir(pDir)) != NULL)
        {
            if(pFile->d_type == 2)		//device
            {
                if(strstr(pFile->d_name,"hidraw")!=NULL)
                {
                    sprintf(path,"/dev/%s",pFile->d_name);
                    get_dev_info(path,usb_info);
                    if(usb_info->vid==3944 && usb_info->pid==22630)	// touch
                    {
                        switch(count)
                        {
                        case 2:
                            event = 4;
                            break;
                        case 1:
                            event = 5;
                            break;
                        case 0:
                            event = 6;
                            break;
                        default:
                            event = 0;
                            break;
                        }
                    }
                    else if((usb_info->vid==2303) && (usb_info->pid==9))	// card reader
                        snprintf(dev_path[0],20,"%s",path);
                    else if((usb_info->vid==1534) && (usb_info->pid==4130))	// new card reader
                        snprintf(dev_path[0],20,"%s",path);
                    else if(usb_info->vid==1155 && usb_info->pid==17)	// scan
                        snprintf(dev_path[1],20,"%s",path);
                    else if(usb_info->vid==8208 && usb_info->pid==30264)	// new scan
                        snprintf(dev_path[1],20,"%s",path);
                    count++;
                    //	printf("vid: %lu\t,pid: %lu\n",usb_info->vid,usb_info->pid);
                    memset(path,0x00,24);
                }
            }
        }
    }
    closedir(pDir);
    free(usb_info);
    return event;
}


