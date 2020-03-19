#ifndef __NETWORK_H
#define __NETWORK_H

#include "sys.h"
#include "usart.h"
#include "delay.h"

//u8 nblot_flag = 0;

/*连接服务器宏定义*/
#define TCP "TCP"
#define UDP "UDP"
#define IP  "103.46.128.43"//花生壳 IP	caisiyu.wicp.vip:56695
#define PORT 56695
//#define IP  "103.46.128.43"//花生壳 IP	2u7919q843.zicp.vip:11967
//#define PORT 11967
//#define IP  "10.101.77.111"//本地 ：内网主机
//#define PORT 8888


/*发送接收缓冲区长度宏定义*/
#define TXBUFFER_LEN 50
#define RXBUFFER_LEN 30

u8 initGPRS(void);
u8 Set_Text_Mode(void);
u8 Send_Text_Sms(char *str);
u8 GPRS_Send_String(uchar *str);
u8 findStr(u8* dest,u8* src,u16 retry_cn);

#endif




//#ifndef __NETWORK_H
//#define __NETWORK_H

//#include "sys.h"

///*连接AP宏定义*/
////#define SSID "ipone"
////#define PWD  "123456321"

//#define SSID "ipone"
//#define PWD  "123456321"

///*连接服务器宏定义*/
//#define TCP "TCP"
//#define UDP "UDP"

////#define Server_IP "169.254.55.128"
////#define Server_IP  "192.168.0.102"
//#define Server_IP  "10.101.77.111"
//#define Server_PORT 8888

//#define TLINK_IP    "112.74.142.132"
//#define TLINK_PORT  8647
//#define APP_ID      "M35F9JXPPQE4SFR1"

////112.74.142.132			8647 	CSY:1,0,1,1,1,1,1,1,1,1,1,1,1,1
////CSY:1:1:1:1:1:1:1:1#
///*发送接收缓冲区长度宏定义*/
//#define TXBUFFER_LEN 50
//#define RXBUFFER_LEN 30

//void sendDeviceStatus(void);
//u8 connect_Server(void);
//u8 checkESP8266(void);
//u8 initESP8266(void);
//void restoreESP8266(void);
//u8 connectAP(u8* ssid,u8* pwd);
//u8 connectServer(u8* mode,u8* ip,u16 port);
//void sendBuffertoServer(u8* buffer);
////void processServerBuffer(jiaju * jiaju);
//u8 disconnectServer(void);
//u8 findStr(u8* dest,u8* src,u16 retry_cn);

//void NBLOT_Process(void);
//#endif
