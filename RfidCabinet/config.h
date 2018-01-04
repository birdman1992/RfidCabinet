#ifndef CONFIG_H
#define CONFIG_H

//PC上测试请注释此条
//#define RUN_IN_ARM

/**************设备名*****************/
//RFID设备
#define DEV_Rfid "/dev/ttymxc3"  //开发板右侧串口
#define DEV_RFID_PC "/dev/ttyUSB1"  //PC串口调试RFID

#define FILE_CONFIG_CABINET_LAYOUT "/home/config/cabinet.layout"



#endif // CONFIG_H
