#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include <QList>
#include <QTimer>
#include "devicesimulator.h"
#include "Qextserial/qextserialport.h"
#include "device/rfiddevice.h"
#include "Structs/rfidchangeinfo.h"
#include "device/Hid/qhid.h"

#define DEV_LOCK_CTRL "/dev/ttymxc2"   //底板串口
#define DEV_RFID_CTRL "/dev/ttymxc4"    //rfid网关串口
//#define DEV_LOCK_CTRL "/dev/ttymxc3"   //开发板右侧串口

typedef struct ui{
    long vid;
    long pid;
}USBINFO;

class DeviceManager : public QObject
{
    Q_OBJECT
public:
    explicit DeviceManager(QObject *parent = NULL);
    ~DeviceManager();

private:
    DeviceSimulator* win_controler;
    QHid* hid_card_reader;//读卡器设备
    QHid* hid_code_scan;//扫码设备
    QTimer* reqTimer;
    bool scanState;
    bool cardReaderState;
    bool lockState;//柜门状态  true:开  false:关
    bool lockStateReq;//
    QextSerialPort* com_lock_ctrl;//柜门锁控制器
    RfidDevice* devRfid;
    void deviceInit();
    QextSerialPort *comCtrlInit(QString devName, int baudRate, int dataBits, int Parity, int stopBits);

    int get_path();
    int get_dev_info(char *dev_name, USBINFO *uInfo);
    void lockCtrl(int seqNum, int ioNum);


signals:
    void rfidIn(QList<rfidChangeInfo*>);
    void rfidOut(QList<rfidChangeInfo*>);
    void lockCtrlData(QByteArray);//暂无
    void cardReaderData(QString);//当前可用
    void codeScanData(QString);//当前可用
    void doorStateChanged(bool);
    void rfidFinish();

public slots:
    void openCabDoor();//打开柜门
    void recvDoorState(bool isopen);
    void insertRfid(QStringList);
    void setLockActive(bool act);
    void lockWarning(int times);
    void test();
private slots:
    void readLockCtrlData();
    void readCardReaderData(QByteArray);
    void readCodeScanData(QByteArray);
    void getLockState();

};

#endif // DEVICEMANAGER_H
