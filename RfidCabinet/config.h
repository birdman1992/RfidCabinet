#ifndef CONFIG_H
#define CONFIG_H

//PC上测试请注释此条
#define RUN_IN_ARM

/**************设备名*****************/
//RFID设备
#define DEV_Rfid "/dev/ttymxc1"  //开发板右侧串口
#define DEV_RFID_PC "/dev/ttyUSB0"  //PC串口调试RFID

#define CAB_TYPE "1111#211112#22112#1"

#define FILE_CONFIG_CABINET_LAYOUT "/home/config/cabinet.layout"
#define FILE_CONFIG_REP "/home/config/"

#define DEFAULT_SERVER "192.168.0.105:8080"


#endif // CONFIG_H
