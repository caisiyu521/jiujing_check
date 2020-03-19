#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "nvic.h"
#include "led.h"
#include "timer.h"
#include "string.h"
#include "usart1.h"	   
#include "gps.h"
#include "adc.h"
#include "oled.h"
#include "key.h"
#include "network.h"

int Str16_To_Hex(char * buf , char countnum ,char * NBLOT_BUFFER_Hex);

uchar caisiyu_flag = 0;

/*					定义关于 定时器 重定义的 变量								*/	
extern uint8_t beep_num ;//用来蜂鸣器报警，定时器中用到 example：beep_num = 2.响1下
u8 nblot_flag;//定时器中用到 ，，延时效果 5s 翻转状态
/*		  																		*/

/*					定义关于 gps的 变量								*/				
nmea_msg gpsx; 											//GPS信息
__align(4) u8 dtbuf[50];   								//打印缓存器
const u8*fixmode_tbl[4]={"Fail","Fail"," 2D "," 3D "};	//fix mode字符串 
float tp;												//用来 存储gps返回数据的 变量
void Gps_Msg_Show(void);								//显示GPS定位信息 
u8 TEMP_BUF[USART1_MAX_RECV_LEN];						//接受 GPS 串口返回的数据
void GPS_INIT(void);									//GPS 模块初始化
void GPS_SHUJU(void);									//GPS 模块数据解析
/*		  																		*/

/*					定义关于 ADC的 变量								*/
extern float ADC_ConvertedValueLocal;    // 局部变量，用于保存转换计算后的电压值 
/*		  															*/

/*					定义关于 NBLOT通讯的 变量								*/
extern u8 TXBuffer[TXBUFFER_LEN];		//nblot 发送缓冲区
extern u8 RXBuffer[RXBUFFER_LEN];		//nblot 接受缓冲区

u8 NBLOT_SHANG_OVERTIME = 0;
/*		  																	*/

/*					字符串转化为 16进制							*/
void StringToHex(char *str, unsigned char *strhex);//将字符串转换为 16进制数
int arrayToStr(unsigned char *buf, unsigned int buflen, char *out);//将16进制数 放在一个 字符串中
char strhex[300] = "";		//定义一个 存储 16进制数的数组指针
char buf[300] = "";			//定义一个 数组 存储区
/*		  																	*/

/*					定义 酒精报警器 的变量							*/
extern uint16_t MQ_Buf;					//存储 当前的酒精浓度
u8 MQ_flag_Time = 0;					//定义 测量时间的变量
u8 alarm_threshold = 20;				//报警阈值，程序可修改
u8 alarm_Value[2];						//存储 输入 阈值 
/*		  																	*/

u16 i,j;										//随意定义的变量 
uint8_t string[200],string2[100],string3[100];	//定义三个输出字符串的字符数组
u8 JIEMIAN = 0;									//切换界面 用的
u8 Shang_Buf[20];								//存储 醉酒值的标识


int main(void){	
	char ttt;		//存储按键 返回键值
	delay_init();	    	//延时函数初始化	  
	uart2_init(115200);	 	//串口初始化为115200
	uart3_init(9600);	 	//串口初始化为115200
	USART1_Init(38400);		//初始化串口1
	LED_Init();				//初始化 LED
	Timer2_Init(1000,72);	//初始化 Timer2 1ms 中断一次
	ADCx_Init();			//ADC 初始化	
	IIC_Init();				//OLED IIC 初始化
	OLED_Init();			//OLED 初始化	
	Key_Init();				//按键 初始化
	
	nvic_init();			//配置 中断的 优先级 ， 放初始化 后面
	
	printf("~~~~~~~~~~~~~~USART2 CPU Start~~~~~~~~~~~~~~~\r\n");
	
	OLED_ON();OLED_CLS();	OLED_CLS();OLED_CLS();//OLED 清屏
	OLED_ShowCHinese(0,2,0);OLED_ShowCHinese(16,2,1);OLED_ShowCHinese(32,2,2);
	OLED_ShowCHinese(48,2,3);OLED_ShowCHinese(64,2,4);OLED_ShowCHinese(80,2,5);
	OLED_ShowCHinese(96,2,6);//OLED显示 开机初始化界面			“系统正在初始化”

//	nblot_flag = 1;
//	while(nblot_flag == 1);
	printf("~~~~~~~~~~~~~~NBLOT INIT~~~~~~~~~~~~~~~\r\n");
//	Usart_SendString(USART3,"AT\r\n");    delay_ms(100);
	if(initGPRS()!=0){printf("nblot_init ok!\r\n");}
	if(Set_Text_Mode()!=0){printf("Server ok!\r\n");}
	caisiyu_flag = 1;
	
	
	GPS_INIT();//GPS初始化
	
	delay_ms(5000);OLED_CLS();
	sprintf((char*)string,"OK...");		
	OLED_ShowStr(25,2,string,2);
	delay_ms(2000);
	OLED_CLS();//显示 初始化结束界面 			“OK...”
	
	beep_num = 4;												//系统运行后 ， 蜂鸣器响2声

//	GPRS_Send_String("adf\r\n");
	
	while(1){
//		NBLOT_Process();
//		GPS_SHUJU();
		
		if(JIEMIAN == 0){				//界面0					//显示功能选项 D 和 C键
			OLED_ShowCHinese(0,2,7);OLED_ShowCHinese(16,2,8);
			sprintf((char*)string,"D");OLED_ShowStr(35,2,string,2);
			OLED_ShowCHinese(45,2,9);OLED_ShowCHinese((45+16),2,10);
			OLED_ShowCHinese((45+32),2,11);OLED_ShowCHinese((45+48),2,12);
			OLED_ShowCHinese((45+64),2,13);					//OLED显示 -- 按下D键开始检测	
			
			OLED_ShowCHinese(0,4,37);OLED_ShowCHinese(16,4,38);
			sprintf((char*)string,"C");OLED_ShowStr(35,4,string,2);
			OLED_ShowCHinese(45,4,39);OLED_ShowCHinese((45+16),4,40);
			OLED_ShowCHinese((45+32),4,41);OLED_ShowCHinese((45+48),4,42);
			OLED_ShowCHinese((45+64),4,43);//OLED显示 -- 				 "按下C键设置报警"
		}
		else if(JIEMIAN == 1){//检测开始计时
			OLED_ShowCHinese(0,2,85);OLED_ShowCHinese((0+16),2,86);OLED_ShowCHinese((0+32),2,87);
			OLED_ShowCHinese((0+48),2,88);OLED_ShowCHinese((0+64),2,89);
			sprintf((char*)string,"%d",MQ_flag_Time);OLED_ShowStr((0+90),2,string,2);
			OLED_ShowCHinese((0+105),2,18);//OLED显示 -- 					"已检测0秒"
		}
		else if(JIEMIAN == 3){//检测完成后
			OLED_ShowCHinese(32,0,19);OLED_ShowCHinese((32+16),0,20);
			OLED_ShowCHinese((32+32),0,21);OLED_ShowCHinese((32+48),0,22);//	"检测完成"
			
			OLED_ShowCHinese(16,2,27);OLED_ShowCHinese((16+16),2,28);OLED_ShowCHinese((16+32),2,30);
			OLED_ShowCHinese((16+48),2,31);OLED_ShowCHinese((16+64),2,34);
			sprintf((char*)string,"%d",MQ_Buf);OLED_ShowStr((16+80),2,string,2);//"当前酒精为17"
			
			if(MQ_Buf > alarm_threshold){//如果 浓度大于阈值 报警
				beep_num = 2;
				OLED_ShowCHinese((0+0),4,25);OLED_ShowCHinese((0+16),4,26);//	"酒驾"

				OLED_ShowCHinese((0+64),4,55);OLED_ShowCHinese((0+80),4,56);
				sprintf((char*)string,":A");OLED_ShowStr((0+105),4,string,2);//	"上传：A"
			}
			else {//如果 浓度小于阈值 没酒驾
				OLED_ShowCHinese((32+0),4,23);OLED_ShowCHinese((32+16),4,25);OLED_ShowCHinese((32+32),4,26);
																				//"没酒驾"
			}
		}
		else if(JIEMIAN == 4){//按键C 按下后 ， 
			OLED_ShowCHinese(0,0,46);OLED_ShowCHinese((0+16),0,47);
			OLED_ShowCHinese((0+32),0,48);OLED_ShowCHinese((0+48),0,49);OLED_ShowCHinese((0+64),0,50);
			sprintf((char*)string,"10-99");OLED_ShowStr((0+80),0,string,2);//"输入报警值10-99"
			
			OLED_ShowCHinese(0,2,46);OLED_ShowCHinese((0+16),2,47);
			sprintf((char*)string,":");OLED_ShowStr((0+32),2,string,2);//		"输入"
			
			OLED_ShowCHinese(0,4,51);OLED_ShowCHinese((0+16),4,52);
			sprintf((char*)string,":A");OLED_ShowStr((0+30),4,string,2);
			OLED_ShowCHinese((0+80),4,53);OLED_ShowCHinese((0+96),4,54);
			sprintf((char*)string,":B");OLED_ShowStr((0+110),4,string,2);//"确定：A 取消：B"
			
			if(i == 1){sprintf((char*)string,"%d",alarm_Value[0]);OLED_ShowStr((0+48),2,string,2);}
			else if(i == 2){sprintf((char*)string,"%d",alarm_Value[1]);OLED_ShowStr((0+60),2,string,2);}
																			//如果按键输入值
		}
		else if(JIEMIAN == 5){
			OLED_ShowCHinese(0,0,58);OLED_ShowCHinese((0+16),0,59);OLED_ShowCHinese((0+32),0,60);
			OLED_ShowCHinese((0+48),0,61);OLED_ShowCHinese((0+64),0,62);OLED_ShowCHinese((0+80),0,63);
			OLED_ShowCHinese((0+96),0,64);OLED_ShowCHinese((0+112),0,65);//请输入酒驾人的标识
			
//			OLED_ShowCHinese(0,2,46);OLED_ShowCHinese((0+16),2,47);
//			sprintf((char*)string,":");OLED_ShowStr(0,2,string,2);//输入
			
			OLED_ShowCHinese(30,5,81);OLED_ShowCHinese((30+16),5,82);
			sprintf((char*)string,":A");OLED_ShowStr((30+30),5,string,2);//确定：A 
			OLED_ShowCHinese((0+80),5,83);OLED_ShowCHinese((0+96),5,84);
			sprintf((char*)string,":B");OLED_ShowStr((0+110),5,string,2);//清除：B
		}
		else if(JIEMIAN == 6){
			OLED_ShowCHinese(0,0,90);OLED_ShowCHinese((0+16),0,91);OLED_ShowCHinese((0+32),0,92);OLED_ShowCHinese((0+48),0,93);
			OLED_ShowCHinese((0+64),0,94);OLED_ShowCHinese((0+80),0,95);//发送数据失败
			
			OLED_ShowCHinese(0,2,96);OLED_ShowCHinese((0+16),2,97);OLED_ShowCHinese((0+32),2,98);OLED_ShowCHinese((0+48),2,99);
			OLED_ShowCHinese((0+64),2,100);
			OLED_ShowCHinese((0+0),4,101);OLED_ShowCHinese((0+16),4,102);OLED_ShowCHinese((0+32),4,103);OLED_ShowCHinese((0+48),4,104);//请检查网络重启设备
			nblot_flag = 1;
			while(nblot_flag == 1);
			OLED_CLS();
			JIEMIAN = 0;
		}
		
		ttt = Key_Scan();//按键扫描
		
		if(ttt == 'D'){//如果按下D键 
			if(JIEMIAN == 0){//开始计时 ， 测量开始
				JIEMIAN = 1;
				MQ_flag_Time = 5;
			}
//			else if(JIEMIAN == 1){//停止计时 ， 测量结束
//				JIEMIAN = 2;
//			}
			OLED_CLS();//刷新一次屏幕
		}
		else if(ttt == '*'){//如果按下*键 , 无条件转回界面0
			OLED_CLS();
			JIEMIAN = 0;
			i = 0;j = 0;//清空 防止对以后的数据有影响
			MQ_Buf =0;//清空测量值
			OLED_CLS();//刷新一次屏幕
		}
		else if(ttt == 'C'){//如果按下C键 
			if(JIEMIAN == 0){//界面0 设置报警值
				OLED_CLS();//刷新一次屏幕
				JIEMIAN = 4;//切换4屏幕 ， 设置报警阈值
				i = 0;j = 0;//清空 防止对以后的数据有影响
			}
		}	
		else if(ttt == 'A'){//如果按下A键
			if(JIEMIAN == 4){//界面4  设置报警阈值 ，A代表确定按钮
				if(((alarm_Value[0]*10+alarm_Value[1])>=10) && ((alarm_Value[0]*10+alarm_Value[1])<100)){
					alarm_threshold = alarm_Value[0]*10+alarm_Value[1];//设置的阈值符合要求的话
					JIEMIAN = 0;//跳转到界面1
					OLED_CLS();//刷新一次屏幕
				}
				else {//如果点击确定后，设定的值不正确 则会清空刚才输入的数据
					alarm_Value[0] = 0;alarm_Value[1] = 0;//清空
					i=0;j = 0;
					sprintf((char*)string,"");OLED_ShowStr((0+48),2,string,2);//清屏
					sprintf((char*)string,"");OLED_ShowStr((0+60),2,string,2);
					OLED_CLS();//刷新一次屏幕
				}
			}
			else if(JIEMIAN == 3){//如果在界面3按下，代表需要将酒驾人的标识 上传至网上
				if(MQ_Buf > alarm_threshold){//再次确定此人是否是酒驾
					OLED_CLS();//清屏
					JIEMIAN = 5;i = 0;j = 0;//确定成功 ，切换界面5
				}
			}
			
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////			
			else if(JIEMIAN == 5){//如果在界面5按下，说明已输入数据 ， 确定上传
				OLED_CLS();
				
				OLED_ShowCHinese(16,2,77);OLED_ShowCHinese(32,2,78);
				OLED_ShowCHinese(48,2,79);OLED_ShowCHinese(64,2,80);//OLED显示 			正在上传
				memset(string2,0,sizeof(string2));
				memset(string,0,sizeof(string));
				GPS_SHUJU();
				if(gpsx.gpssta > 0){
					sprintf((char*)string,":%s:%d",Shang_Buf,MQ_Buf);
					tp=gpsx.longitude;	   
					sprintf((char *)string2,", Longitude is = %.5f %1c ",tp/=100000,gpsx.ewhemi);	//得到经度字符串
					printf("%s	\r\n",dtbuf);
					strcat((char *)string,(char *)string2);
					tp=gpsx.latitude;	   
					sprintf((char *)string2," Latitude is = %.5f %1c \r\n",tp/=100000,gpsx.nshemi);	//得到纬度字符串
					printf("%s	\r\n",dtbuf);
					strcat((char *)string,(char *)string2);
				}
				else {
//					sprintf((char*)string,"The identity of the character is = %s , Alcohol concentration is = %d",Shang_Buf,MQ_Buf);
					sprintf((char*)string,":%s:%d:0:0",Shang_Buf,MQ_Buf);
				}
				printf("caiji string = %s\r\n",string);
//				StringToHex((char *)string,(u8 *)strhex);
//				arrayToStr((u8 *)strhex, strlen(strhex), buf);
//				sprintf((char*)string,"AT+NMGS=%d,%s\r\n",strlen(strhex),buf);
//				printf("the hex is:%s\n",string);
				memset(RXBuffer,0,sizeof(RXBuffer));
//				Usart_SendString(USART3,(char *)string);  
				if(GPRS_Send_String(string) == 0){
					JIEMIAN = 6;				//切换到界面6中 ， 提示发送失败！！！！
				}
				
//				nblot_flag = 1;
//				while(nblot_flag == 1);
//				while(findStr(RXBuffer,(u8 *)"NSMI:SENT",200)==0)   { 
//					Usart_SendString(USART3,(char *)string);   
//					printf("Send error\r\n");
//					
//					nblot_flag = 1;			//延时
//					while(nblot_flag == 1);
//					
//					if(++NBLOT_SHANG_OVERTIME == 5){//超过 50s还没有连接上服务器 ， 则退出
//						NBLOT_SHANG_OVERTIME = 0;
//						JIEMIAN = 6;				//切换到界面6中 ， 提示发送失败！！！！
//						OLED_CLS();
//						beep_num = 10;
//						break;
//					}
//				}
				if(JIEMIAN != 6){//如果并没有 网络错误，
					printf("Send ok\r\n");
					beep_num = 6;
					
					memset(Shang_Buf,0,sizeof(Shang_Buf));				
					
					JIEMIAN = 0;i = 0;j = 0;//上传成功 返回界面0
					OLED_CLS();
				}
			}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		}
		else if(ttt == 'B'){//如果按下B键
			if(JIEMIAN == 4){//如果在界面4按下，说明不想要在重新设定报警值，直接返回主界面0
				JIEMIAN = 0;
				OLED_CLS();
				alarm_Value[0] = 0;alarm_Value[1] = 0;
				i=0;
			}
			else if(JIEMIAN == 5){//如果在界面4按下，说明将重新输入 酒驾人的标识
				for(j = 0;j < 10;j++){//把之前输入的数据清空掉
					sprintf((char*)string,"");OLED_ShowStr((48+(j*8)),2,string,2);
				}
				OLED_CLS();
				i = 0;j = 0;
				memset(Shang_Buf,0,sizeof(Shang_Buf));//清空上传的数组
			}
		}
		else if((JIEMIAN == 4)&&(ttt != '&')&&(ttt != 'A')&&(ttt != 'B')&&(ttt != 'C')&&(ttt != 'D')&&(ttt != '#')&&(ttt != '*')){
			alarm_Value[i] = ttt-'0';
			i+=1;
		}
		else if((JIEMIAN == 5)&&(ttt != '&')&&(ttt != 'A')&&(ttt != 'B')&&(ttt != 'C')&&(ttt != 'D')&&(ttt != '#')&&(ttt != '*')){
			if(j < 14){
				sprintf((char*)string,"%c",ttt);OLED_ShowStr(((j*8)),2,string,2); j++;
				Shang_Buf[i] = ttt;
			}
			else if(j >= 14){
				sprintf((char*)string,"%c",ttt);OLED_ShowStr((((j-14)*8)),5,string,2); j++;
				Shang_Buf[i] = ttt;
			}
//			sprintf((char*)string,"%c",ttt);OLED_ShowStr(((j*8)),2,string,2); j++;
//			Shang_Buf[i] = ttt;
			i+=1;
			Shang_Buf[i] = '\0';
			printf("Shang_Buf = %s\r\n",Shang_Buf);
		}
		
//		GPS_SHUJU();
	}
}

void GPS_SHUJU(void){
	u16 rxlen , lenx;
	if(USART1_RX_STA&0x8000){					   
		rxlen=USART1_RX_STA&0X7FFF;	//得到数据长度
//		printf("aaaaaaaa%s\r\n",USART1_RX_BUF);
		for(i=0;i<rxlen;i++)TEMP_BUF[i]=USART1_RX_BUF[i];	   
		USART1_RX_STA=0;		   	//启动下一次接收
		TEMP_BUF[i]=0;				//自动添加结束符
		GPS_Analysis(&gpsx,(u8*)TEMP_BUF);//分析字符串
		Gps_Msg_Show();
	}
//		if((lenx%500)==0);
//			LED0=!LED0
		lenx++;	
}

void GPS_INIT(void){
	u8 key = 1;
	if(Ublox_Cfg_Rate(1000,1)!=0)	//设置定位信息更新速度为1000ms,顺便判断GPS模块是否在位. 
	{
		printf("NEO-6M Setting...\r\n");
		while((Ublox_Cfg_Rate(1000,1)!=0)&&key)	//持续判断,直到可以检查到NEO-6M,且数据保存成功
		{
			USART1_Init(9600);				//初始化串口1波特率为9600(EEPROM没有保存数据的时候,波特率为9600.)
	  		Ublox_Cfg_Prt(38400);			//重新设置模块的波特率为38400
			USART1_Init(38400);				//初始化串口1波特率为38400 
			Ublox_Cfg_Tp(1000000,100000,1);	//设置PPS为1秒钟输出1次,脉冲宽度为100ms	    
			key=Ublox_Cfg_Cfg_Save();		//保存配置  
		}	  					 
		printf("NEO-6M Set Done!\r\n");
		delay_ms(500);
	}
}

//显示GPS定位信息 
void Gps_Msg_Show(void)
{
 	float tp;		
	printf("\r\n\r\n\n\n\n\n");    
	tp=gpsx.longitude;	   
	sprintf((char *)dtbuf,"Longitude:%.5f %1c   \r\n",tp/=100000,gpsx.ewhemi);	//得到经度字符串
	printf("%s	\r\n",dtbuf);
	tp=gpsx.latitude;	   
	sprintf((char *)dtbuf,"Latitude:%.5f %1c   ",tp/=100000,gpsx.nshemi);	//得到纬度字符串
	printf("%s	\r\n",dtbuf);
	tp=gpsx.altitude;	   
 	sprintf((char *)dtbuf,"Altitude:%.1fm     ",tp/=10);	    			//得到高度字符串
	printf("%s	\r\n",dtbuf);
	tp=gpsx.speed;	   
 	sprintf((char *)dtbuf,"Speed:%.3fkm/h     ",tp/=1000);		    		//得到速度字符串	 
	printf("%s	\r\n",dtbuf);
	if(gpsx.fixmode<=3)														//定位状态
	{  
		sprintf((char *)dtbuf,"Fix Mode:%s",fixmode_tbl[gpsx.fixmode]);	
		printf("%s	\r\n",dtbuf);
	}	 	   
	sprintf((char *)dtbuf,"Valid satellite:%02d",gpsx.posslnum);	 		//用于定位的卫星数
	printf("%s	\r\n",dtbuf);
	sprintf((char *)dtbuf,"Visible satellite:%02d",gpsx.svnum%100);	 		//可见卫星数
	printf("%s	\r\n",dtbuf);
	printf("%s	\r\n",dtbuf);	
	sprintf((char *)dtbuf,"UTC Date:%04d/%02d/%02d   ",gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//显示UTC日期
	printf("year2:%d\r\n",gpsx.utc.year);
	printf("%s	\r\n",dtbuf);	
	sprintf((char *)dtbuf,"UTC Time:%02d:%02d:%02d   ",gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//显示UTC时间
	printf("%s	\r\n",dtbuf);
}

void StringToHex(char *str, unsigned char *strhex){
	unsigned char  i, cnt = 0;
	char *p = str;             //直针p初始化为指向str
	unsigned char len = strlen(str); //获取字符串中的字符个数

	while (*p != '\0') {        //结束符判断
		for (i = 0; i < len; i++)  //循环判断当前字符是数字还是小写字符还是大写字母
		{
			if ((*p >= '0') && (*p <= '9')) //当前字符为数字0~9时
				strhex[cnt] = *p - '0' + 0x30;//转为十六进制

			if ((*p >= 'A') && (*p <= 'Z')) //当前字符为大写字母A~Z时
				strhex[cnt] = *p - 'A' + 0x41;//转为十六进制

			if ((*p >= 'a') && (*p <= 'z')) //当前字符为小写字母a~z时
				strhex[cnt] = *p - 'a' + 0x61;  //转为十六进制
			if (*p == ':') //当前字符为小写字母a~z时
				strhex[cnt] = 0x3A;  //转为十六进制
			if (*p == ' ') //当前字符为小写字母a~z时
				strhex[cnt] = 0x20;  //转为十六进制
			if (*p == '=') //当前字符为小写字母a~z时
				strhex[cnt] = 0x3D;  //转为十六进制
			if (*p == ',') //当前字符为小写字母a~z时
				strhex[cnt] = 0x2C;  //转为十六进制
			if (*p == '.') //当前字符为小写字母a~z时
				strhex[cnt] = 0x2E;  //转为十六进制
			p++;    //指向下一个字符
			cnt++;
		}
	}
}

int arrayToStr(unsigned char *buf, unsigned int buflen, char *out){
	char strBuf[400] = {0};
	char pbuf[400];
	int i;
	for (i = 0; i < buflen; i++)
	{
		sprintf(pbuf, "%02X", buf[i]);
		strncat(strBuf, pbuf, 2);
	}
	strncpy(out, strBuf, buflen * 2);
	//printf("out = %s\n", out);
	return buflen * 2;
}

int Str16_To_Hex(char * buf , char countnum ,char * NBLOT_BUFFER_Hex){
	int i  = 0, j = 0 , count = 0;
	for(i = 0; i < countnum;i++){
		buf[i] = (string[j]-'0') << 4 | (string[j+1] - '0');
		j += 2;
		count ++;
	}
	return count;
}

