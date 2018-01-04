#include "rfiddevice.h"
extern "C"{
#include "ber_test/inc/RfidApi.h"
}
#include "config.h"
#include <QDebug>

RfidDevice::RfidDevice(QObject *parent) : QThread(parent)
{
//    runFlag = false;

    rInfo = (PMDYINFO)malloc(sizeof(MDYINFO)*MHI);
    memset(rInfo,0x00,sizeof(sizeof(MDYINFO)*MHI));

    setbuf(stdout,NULL);

    cfd = open_com(QByteArray(DEV_RFID_PC).data());
    if(cfd > 0)
        qDebug()<<DEV_RFID_PC<<"open success!";
    CryptTables();				// 初始化hash
    RfidHash = inithashtable(MHI);
}

RfidDevice::~RfidDevice()
{
    if(this->isRunning())
        stopScan();

    free(rInfo);
    FreeHash(RfidHash);
}

void RfidDevice::startScan()
{
    qDebug()<<"[startScan]";
    if(!this->isRunning())
        this->start();
}

void RfidDevice::stopScan()
{
    qDebug()<<"[stopScan]";
    runFlag = false;
//    this->quit();
//    this->wait();
}


//void RfidDevice::run()
//{
//    int i;
//    runFlag = true;

//    while(runFlag)
//    {
//        sleep(5);
//        RFID_DATA *epc = (RFID_DATA *)malloc(sizeof(RFID_DATA));
//        GetEpc(epc);
//        if(epc != NULL)
//        {
//            while (epc->next != NULL)
//            {
//                epc = epc->next;
//                printf("id[ANT%d][%d]: ",epc->ant_num,epc->dc_type);
//                for(i=0;i<12;i++)
//                    printf("%02X  ",epc->id[i]);
//                printf("\n");
//            }
//        }
//        printf("\n");
//        printf("\n");
//        FreeRS(epc);
//    }
//    return;
//}

void RfidDevice::run()
{
//    int i;
    QByteArray id;
    runFlag = true;

    while(runFlag)
    {
        RFID_DATA *epc = (RFID_DATA *)malloc(sizeof(RFID_DATA));
        GetEpc(epc);
        if(epc != NULL)
        {
            while (epc->next != NULL)
            {
                epc = epc->next;
                if(epc->dc_type == 1)
                {
                    id = QByteArray((const char*)epc->id, 12);
                    emit rfidIn(epc->ant_num,id);
                    qDebug()<<"[RFID in]"<<"ant:"<<epc->ant_num<<id.toHex();
                }
                else if(epc->dc_type == 0)
                {
                    id = QByteArray((const char*)epc->id, 12);
                    emit rfidOut(epc->ant_num,id);
                    qDebug()<<"[RFID out]"<<"ant:"<<epc->ant_num<<id.toHex();
                }
//                printf("id[ANT%d][%d]: ",epc->ant_num,epc->dc_type);
//                for(i=0;i<12;i++)
//                    printf("%02X  ",epc->id[i]);
//                printf("\n");
            }
        }
//        printf("\n");
//        printf("\n");
        FreeRS(epc);
        sleep(5);
    }
    qDebug()<<"[RfidDevice]:scan finished.";
    return;
}
