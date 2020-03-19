#include "network.h"
#include "led.h"
#include "usart.h"
#include "delay.h"
#include "string.h"

uchar  GPRS_Delay_1S = 0;

//unsigned char *SMS_Num="\"+8613800377500\"";//短信中心号 关于中心号的介绍可以看串口调试笔记的第二部分
//unsigned char *phone="\"15517175421\"";     //短信接收方手机号
//unsigned char *content="Thanks";//发送短信内容
const char *string123 = "AT+CIPSTART=\"TCP\",\"103.46.128.43\",56695\r\n";	//IP登录服务器

u8 TXBuffer[TXBUFFER_LEN] = {0};  //网络通信数据发送缓冲
u8 RXBuffer[RXBUFFER_LEN] = {0};  //网络通信数据接收缓冲
uchar OVER = 0;	

u8 initGPRS(void){
		//防止一直在循环
	Usart_SendString(USART3,"AT\r\n");     //测试GPRS模块与单片机是否连接
	if(findStr(RXBuffer,(u8 *)"OK",10)==0)   { 
		Usart_SendString(USART3,"AT\r\n"); 
		GPRS_Delay_1S = 1;while(GPRS_Delay_1S);				//精确延时1S
		if(findStr(RXBuffer,(u8 *)"OK",40)==0)   {
			printf("AT测试指令错误\r\n");
			return 0;  
		}     
    }
	memset(RXBuffer,0,sizeof(RXBuffer));
	
	Usart_SendString(USART3,"AT+CREG?\r\n");   		//查询模块是否注册成功		比较重要
	GPRS_Delay_1S = 1;while(GPRS_Delay_1S);			//精确延时1S
	OVER  = 0;
	while(findStr(RXBuffer,(u8 *)"+CREG: 0,1",40)==0){ //如果没有收到发送成功的回复则继续发送
		Usart_SendString(USART3,"AT+CREG?\r\n");	//精确延时1S
		printf("Send error\r\n");
		if(++OVER == 20)	return 0;				//超过10秒还没有连接成功 则退出
    }
	memset(RXBuffer,0,sizeof(RXBuffer));
	printf("zhuce success ok\r\n");
	
    return 1;                         //设置成功
}

u8 Set_Text_Mode(void)
{
	Usart_SendString(USART3,"ATE0\r\n");			//关闭回显
	if(findStr(RXBuffer,(unsigned char*)"OK",40)==0) {
        return 0;      
    }
	
	memset(RXBuffer,0,sizeof(RXBuffer));
	Usart_SendString(USART3,"AT+CIPSHUT\r\n");		//关闭移动场景		
	if(findStr(RXBuffer,(unsigned char*)"OK",40)==0) {
        return 0;      
    }
	
	memset(RXBuffer,0,sizeof(RXBuffer));
	Usart_SendString(USART3,"AT+CGCLASS=\"B\"\r\n");//设置GPRS移动台类别为B,支持包交换和数据交换 
	GPRS_Delay_1S = 1;while(GPRS_Delay_1S);
	if(findStr(RXBuffer,(unsigned char*)"OK",50)==0) {
        return 0;      
    }
	
	memset(RXBuffer,0,sizeof(RXBuffer));
	Usart_SendString(USART3,"AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");//设置PDP上下文,互联网接协议,接入点等信息
	if(findStr(RXBuffer,(unsigned char*)"OK",40)==0) {
        return 0;      
    }
	
	memset(RXBuffer,0,sizeof(RXBuffer));
	Usart_SendString(USART3,"AT+CGATT=1\r\n");	//附着GPRS业务
	if(findStr(RXBuffer,(unsigned char*)"OK",40)==0) {
        return 0;      
    }
	
	memset(RXBuffer,0,sizeof(RXBuffer));
	Usart_SendString(USART3,"AT+CIPCSGP=1,\"CMNET\"\r\n");	//设置为GPRS连接模式
	if(findStr(RXBuffer,(unsigned char*)"OK",40)==0) {
        return 0;      
    }
	
	
	
	memset(RXBuffer,0,sizeof(RXBuffer));
	Usart_SendString(USART3,"AT+CIPHEAD=1\r\n");	//设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)
	if(findStr(RXBuffer,(unsigned char*)"OK",40)==0) {
        return 0;      
    }
	
	memset(RXBuffer,0,sizeof(RXBuffer));
	Usart_SendString(USART3,(char*)string123);				//设置为GPRS连接模式
	while(findStr(RXBuffer,(u8 *)"OK",40)==0){ //如果没有收到发送成功的回复则继续发送
		printf("Send error\r\n");
		Usart_SendString(USART3,(char*)string123);				//设置为GPRS连接模式
		if(++OVER == 5)	return 0;				//超过10秒还没有连接成功 则退出
    }
	
	OVER = 0;
	while(findStr(RXBuffer,(u8 *)"CONNECT OK",40)==0){ //如果没有收到发送成功的回复则继续发送
		printf("Send error\r\n");
		if(++OVER == 5)	return 0;				//超过10秒还没有连接成功 则退出
    }
	return 1;   
}

u8 GPRS_Send_String(uchar *str){
	Usart_SendString(USART3,"AT+CIPSEND\r\n");
	GPRS_Delay_1S = 1;while(GPRS_Delay_1S);
	GPRS_Delay_1S = 1;while(GPRS_Delay_1S);
	
	Usart_SendString(USART3,(char *)str);	//设置为GPRS连接模式
	GPRS_Delay_1S = 1;while(GPRS_Delay_1S);
	GPRS_Delay_1S = 1;while(GPRS_Delay_1S);		//精确延时1S
	USART_SendData(USART3,0X1A);	//设置为GPRS连接模式
	OVER = 0;
	while(findStr(RXBuffer,(u8 *)"SEND OK",10)==0){ //如果没有收到发送成功的回复则继续发送
		printf("Send error\r\n");
		GPRS_Delay_1S = 1;while(GPRS_Delay_1S);		//精确延时1S
		if(++OVER == 10)	return 0;				//超过10秒还没有连接成功 则退出
    }
	return 1;
}

//u8 Set_Text_Mode(void)
//{
//	char temp[50]="AT+CSCA=";							
//	Usart_SendString(USART3,"ATE0\r\n");				//关闭回显
//	if(findStr(RXBuffer,(unsigned char*)"OK",200)==0) {
//        return 0;      
//    }
//	memset(RXBuffer,0,sizeof(RXBuffer));
//	Usart_SendString(USART3,"AT+CMGF=1\r\n");			
//	if(findStr(RXBuffer,(unsigned char*)"OK",200)==0) {
//        return 0;      
//    }
//	memset(RXBuffer,0,sizeof(RXBuffer));
//	Usart_SendString(USART3,"AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n");
//	if(findStr(RXBuffer,(unsigned char*)"OK",200)==0) {
//        return 0;      
//    }
//	memset(RXBuffer,0,sizeof(RXBuffer));

//	strcat(temp,(char*)SMS_Num);				//设置中心号
//	strcat(temp,"\r\n");
//	Usart_SendString(USART3,temp);
//	if(findStr(RXBuffer,(unsigned char*)"OK",200)==0) {
//        return 0;      
//    }
//	memset(RXBuffer,0,sizeof(RXBuffer));
//	
//	return 1;   

//}

//u8 Send_Text_Sms(char *str)
//{
//	char temp[50]="AT+CMGS=";
//	
//	memset(RXBuffer,0,sizeof(RXBuffer));
//	
//	strcat(temp,(char*)phone); //字符串拼接函数（库函数）
//	strcat(temp,"\r\n");
//	Usart_SendString(USART3,temp);
//	GPRS_Delay_1S = 1;while(GPRS_Delay_1S);
//	
//	if(findStr(RXBuffer,(unsigned char*)">",200)==0) {
//		printf("no > \r\n");
//        return 0;      
//		
//    }
//	Usart_SendString(USART3,(char*)str);
//	GPRS_Delay_1S = 1;while(GPRS_Delay_1S);
//	
//	memset(RXBuffer,0,sizeof(RXBuffer));
//	
//	USART_SendData(USART3 ,0X1A);  //发送结束符
////	Usart_SendString(USART3,"\r\n");
//	
//	OVER  = 0;
////	
//	while(findStr(RXBuffer,(u8 *)"+CMGS",200)==0){ //如果没有收到发送成功的回复则继续发送
//		printf("Send error\r\n");
//		GPRS_Delay_1S = 1;while(GPRS_Delay_1S);		//精确延时1S
//		GPRS_Delay_1S = 1;while(GPRS_Delay_1S);		//精确延时1S
//		if(++OVER == 10)	
//			return 0;				//超过10秒还没有连接成功 则退出
//    }
//	memset(RXBuffer,0,sizeof(RXBuffer)); 
//	return 1;
//}


u8 findStr(u8* dest,u8* src,u16 retry_cn)
{
	u16 retry = retry_cn;                   //超时时间
	u8 result_flag = 0;                     //查找结果
	
	while(strstr((const char *)dest,(const char *)src)==0 && --retry!=0){//等待串口接收完毕或超时退出
		GPRS_Delay_1S = 1;while(GPRS_Delay_1S);
	}
	if(retry==0){                     //如果超时则有问题,此时返回0
	   return 0;
	}
	result_flag = 1;                        //执行到这里说明一切正常, 表示查找成功

	if(result_flag){
		return 1;
	}else {
		return 0;
	}
} 



//#include "network.h"
//#include "led.h"
//#include "usart.h"
//#include "delay.h"

//u8 nblot_Flag = 0;
//u8 nblot_flag = 0;
//u8 TXBuffer[TXBUFFER_LEN] = {0};  //网络通信数据发送缓冲
//u8 RXBuffer[RXBUFFER_LEN] = {0};  //网络通信数据接收缓冲
//u8 nblot_Buf[50];
//extern u8 NBLOT_SHANG_OVERTIME;

//u8 findStr(u8* dest,u8* src,u16 retry_cn)
//{
//	u16 retry = retry_cn;                   //超时时间
//	u8 result_flag = 0;                     //查找结果
//	
//	while(strstr((const char *)dest,(const char *)src)==0 && --retry!=0){//等待串口接收完毕或超时退出
//		Delay_ms(10);
//	}

//	if(retry==0){                     //如果超时则有问题,此时返回0
//	   return 0;
//	}
//	result_flag = 1;                        //执行到这里说明一切正常, 表示查找成功

//	if(result_flag){
//		return 1;
//	}else {
//		return 0;
//	}
//}

///**
// * 功能：初始化ESP8266
// * 参数：None
// * 返回值：初始化结果，非0为初始化成功,0为失败
// */

//u8 initESP8266(void){
//	Usart_SendString(USART3,"AT\r\n");    delay_ms(100);  //测试nblot模块与单片机是否连接
//	if(findStr(RXBuffer,(u8 *)"OK",200)==0)   { 
//		Usart_SendString(USART3,"AT\r\n"); 
//		nblot_flag = 1;
//		while(nblot_flag == 1);
//		if(findStr(RXBuffer,(u8 *)"OK",200)==0)   {
//			printf("AT error\r\n");
//			return 0;  
//		}     
//    }
//	memset(RXBuffer,0,sizeof(RXBuffer));
//	printf("test ok\r\n");
//	
//	Usart_SendString(USART3,"AT+CGSN=1\r\n");   delay_ms(100);//检测nblot是否已经插卡
//	if(findStr(RXBuffer,(u8 *)"OK",200)==0)   { 
//		printf("read card error\r\n");
//        return 0;      
//    }
//	memset(RXBuffer,0,sizeof(RXBuffer));
//	printf("read card ok\r\n");
//	
//	Usart_SendString(USART3,"AT+NCDP=117.60.157.137,5683\r\n");    delay_ms(100); //让nblot用coap协议连接透传云服务器
//	if(findStr(RXBuffer,(u8 *)"OK",200)==0){ 
//		printf("AT+NCDP error\r\n");
//        return 0;      
//    }
//	printf("ip config ok\r\n");
//	
//	memset(RXBuffer,0,sizeof(RXBuffer));
//	Usart_SendString(USART3,"AT+NRB\r\n");//重启nblot模块
//	nblot_flag = 1;
//	while(nblot_flag == 1);
//	nblot_flag = 1;
//	while(nblot_flag == 1);
//	printf("chongqi config ok\r\n");
//	
//	memset(RXBuffer,0,sizeof(RXBuffer));
//	
//	Usart_SendString(USART3,"AT+NSMI=1\r\n");   delay_ms(100);//使能消息发送通知
//	if(findStr(RXBuffer,(u8 *)"OK",200)==0)   { 
//		printf("NSMI card error\r\n");
//        return 0;      
//    }
//	printf("NSMI config ok\r\n");
//	
//	Usart_SendString(USART3,"AT+NNMI=2\r\n");   delay_ms(100);//使能消息接收通知
//	if(findStr(RXBuffer,(u8 *)"OK",200)==0)   { 
//		printf("NNMI card error\r\n");
//        return 0;      
//    }
//	printf("NNMI config ok\r\n");
////	
//	Usart_SendString(USART3,"AT+NMGS=4,496E6974\r\n");//发一条消息登录透传云（激活，透传云收到一条消息则开始连接设备）
//	nblot_flag = 1;//延时5秒钟，定时器延时比较准确
//	while(nblot_flag == 1);
//	while(findStr(RXBuffer,(u8 *)"NSMI:SENT",200)==0)   { //如果没有收到发送成功的回复则继续发送
//		Usart_SendString(USART3,"AT+NMGS=4,496E6974\r\n");
//		printf("Send error\r\n");
//		nblot_flag = 1;			//延时5秒钟
//		while(nblot_flag == 1);
//		if(++NBLOT_SHANG_OVERTIME == 10){//超过 50s还没有连接上服务器 ， 则退出
//			NBLOT_SHANG_OVERTIME = 0;
//			return 0;
//		}
//    }
//	
////	AT+NMGR
//	printf("Send  ok\r\n");
//	memset(RXBuffer,0,sizeof(RXBuffer));
//    return 1;                         //设置成功
//}

//void NBLOT_Process(void){
////	static int flag1 = 0 ,flag2 = 0;
//	if(findStr(RXBuffer,(u8 *)"+NNMI",10))   {
//		Usart_SendString(USART3,"AT+NMGR\r\n");Delay_ms (100);
//		if((strstr((const char *)RXBuffer,"8,010600020001E9CA"))||(strstr((const char *)RXBuffer,"8,010300000001840A"))){
//			LED0 = !LED0;
////			if(flag1 == 1){				  //010300010001D5CA 010300000001840A	010300010001D5CA 010300010001D5CA
////				flag1 = 0;
////				Usart_SendString(USART3,"AT+NMGS=7,010302004D7871\r\n");//77
////			}
////			else if(flag1 == 0){
////				flag1 = 1;
////				Usart_SendString(USART3,"AT+NMGS=7,0103020063F86D\r\n");//99
////			}
//			printf("LED!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
////			Str16_To_Hex("010300000001840A",)
//		}									
////		if(strstr((const char *)RXBuffer,"8,010300000002C40B")){
////			if(flag2 == 1){				  //010300000002C40B	 010300000001840A
////				flag2 = 0;
////				Usart_SendString(USART3,"AT+NMGS=7,01030200117848\r\n");//11
////			}
////			else if(flag2 == 0){
////				flag2 = 1;
////				Usart_SendString(USART3,"AT+NMGS=7,0103020058B9BE\r\n");//88
////			}
////			printf("LED****************************\r\n");
//////			Str16_To_Hex("010300000001840A",)
////		}
//    }
//	memset(RXBuffer,0,sizeof(RXBuffer));
//}


