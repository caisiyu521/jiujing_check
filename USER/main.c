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

/*					������� ��ʱ�� �ض���� ����								*/	
extern uint8_t beep_num ;//������������������ʱ�����õ� example��beep_num = 2.��1��
u8 nblot_flag;//��ʱ�����õ� ������ʱЧ�� 5s ��ת״̬
/*		  																		*/

/*					������� gps�� ����								*/				
nmea_msg gpsx; 											//GPS��Ϣ
__align(4) u8 dtbuf[50];   								//��ӡ������
const u8*fixmode_tbl[4]={"Fail","Fail"," 2D "," 3D "};	//fix mode�ַ��� 
float tp;												//���� �洢gps�������ݵ� ����
void Gps_Msg_Show(void);								//��ʾGPS��λ��Ϣ 
u8 TEMP_BUF[USART1_MAX_RECV_LEN];						//���� GPS ���ڷ��ص�����
void GPS_INIT(void);									//GPS ģ���ʼ��
void GPS_SHUJU(void);									//GPS ģ�����ݽ���
/*		  																		*/

/*					������� ADC�� ����								*/
extern float ADC_ConvertedValueLocal;    // �ֲ����������ڱ���ת�������ĵ�ѹֵ 
/*		  															*/

/*					������� NBLOTͨѶ�� ����								*/
extern u8 TXBuffer[TXBUFFER_LEN];		//nblot ���ͻ�����
extern u8 RXBuffer[RXBUFFER_LEN];		//nblot ���ܻ�����

u8 NBLOT_SHANG_OVERTIME = 0;
/*		  																	*/

/*					�ַ���ת��Ϊ 16����							*/
void StringToHex(char *str, unsigned char *strhex);//���ַ���ת��Ϊ 16������
int arrayToStr(unsigned char *buf, unsigned int buflen, char *out);//��16������ ����һ�� �ַ�����
char strhex[300] = "";		//����һ�� �洢 16������������ָ��
char buf[300] = "";			//����һ�� ���� �洢��
/*		  																	*/

/*					���� �ƾ������� �ı���							*/
extern uint16_t MQ_Buf;					//�洢 ��ǰ�ľƾ�Ũ��
u8 MQ_flag_Time = 0;					//���� ����ʱ��ı���
u8 alarm_threshold = 20;				//������ֵ��������޸�
u8 alarm_Value[2];						//�洢 ���� ��ֵ 
/*		  																	*/

u16 i,j;										//���ⶨ��ı��� 
uint8_t string[200],string2[100],string3[100];	//������������ַ������ַ�����
u8 JIEMIAN = 0;									//�л����� �õ�
u8 Shang_Buf[20];								//�洢 ���ֵ�ı�ʶ


int main(void){	
	char ttt;		//�洢���� ���ؼ�ֵ
	delay_init();	    	//��ʱ������ʼ��	  
	uart2_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	uart3_init(9600);	 	//���ڳ�ʼ��Ϊ115200
	USART1_Init(38400);		//��ʼ������1
	LED_Init();				//��ʼ�� LED
	Timer2_Init(1000,72);	//��ʼ�� Timer2 1ms �ж�һ��
	ADCx_Init();			//ADC ��ʼ��	
	IIC_Init();				//OLED IIC ��ʼ��
	OLED_Init();			//OLED ��ʼ��	
	Key_Init();				//���� ��ʼ��
	
	nvic_init();			//���� �жϵ� ���ȼ� �� �ų�ʼ�� ����
	
	printf("~~~~~~~~~~~~~~USART2 CPU Start~~~~~~~~~~~~~~~\r\n");
	
	OLED_ON();OLED_CLS();	OLED_CLS();OLED_CLS();//OLED ����
	OLED_ShowCHinese(0,2,0);OLED_ShowCHinese(16,2,1);OLED_ShowCHinese(32,2,2);
	OLED_ShowCHinese(48,2,3);OLED_ShowCHinese(64,2,4);OLED_ShowCHinese(80,2,5);
	OLED_ShowCHinese(96,2,6);//OLED��ʾ ������ʼ������			��ϵͳ���ڳ�ʼ����

//	nblot_flag = 1;
//	while(nblot_flag == 1);
	printf("~~~~~~~~~~~~~~NBLOT INIT~~~~~~~~~~~~~~~\r\n");
//	Usart_SendString(USART3,"AT\r\n");    delay_ms(100);
	if(initGPRS()!=0){printf("nblot_init ok!\r\n");}
	if(Set_Text_Mode()!=0){printf("Server ok!\r\n");}
	caisiyu_flag = 1;
	
	
	GPS_INIT();//GPS��ʼ��
	
	delay_ms(5000);OLED_CLS();
	sprintf((char*)string,"OK...");		
	OLED_ShowStr(25,2,string,2);
	delay_ms(2000);
	OLED_CLS();//��ʾ ��ʼ���������� 			��OK...��
	
	beep_num = 4;												//ϵͳ���к� �� ��������2��

//	GPRS_Send_String("adf\r\n");
	
	while(1){
//		NBLOT_Process();
//		GPS_SHUJU();
		
		if(JIEMIAN == 0){				//����0					//��ʾ����ѡ�� D �� C��
			OLED_ShowCHinese(0,2,7);OLED_ShowCHinese(16,2,8);
			sprintf((char*)string,"D");OLED_ShowStr(35,2,string,2);
			OLED_ShowCHinese(45,2,9);OLED_ShowCHinese((45+16),2,10);
			OLED_ShowCHinese((45+32),2,11);OLED_ShowCHinese((45+48),2,12);
			OLED_ShowCHinese((45+64),2,13);					//OLED��ʾ -- ����D����ʼ���	
			
			OLED_ShowCHinese(0,4,37);OLED_ShowCHinese(16,4,38);
			sprintf((char*)string,"C");OLED_ShowStr(35,4,string,2);
			OLED_ShowCHinese(45,4,39);OLED_ShowCHinese((45+16),4,40);
			OLED_ShowCHinese((45+32),4,41);OLED_ShowCHinese((45+48),4,42);
			OLED_ShowCHinese((45+64),4,43);//OLED��ʾ -- 				 "����C�����ñ���"
		}
		else if(JIEMIAN == 1){//��⿪ʼ��ʱ
			OLED_ShowCHinese(0,2,85);OLED_ShowCHinese((0+16),2,86);OLED_ShowCHinese((0+32),2,87);
			OLED_ShowCHinese((0+48),2,88);OLED_ShowCHinese((0+64),2,89);
			sprintf((char*)string,"%d",MQ_flag_Time);OLED_ShowStr((0+90),2,string,2);
			OLED_ShowCHinese((0+105),2,18);//OLED��ʾ -- 					"�Ѽ��0��"
		}
		else if(JIEMIAN == 3){//�����ɺ�
			OLED_ShowCHinese(32,0,19);OLED_ShowCHinese((32+16),0,20);
			OLED_ShowCHinese((32+32),0,21);OLED_ShowCHinese((32+48),0,22);//	"������"
			
			OLED_ShowCHinese(16,2,27);OLED_ShowCHinese((16+16),2,28);OLED_ShowCHinese((16+32),2,30);
			OLED_ShowCHinese((16+48),2,31);OLED_ShowCHinese((16+64),2,34);
			sprintf((char*)string,"%d",MQ_Buf);OLED_ShowStr((16+80),2,string,2);//"��ǰ�ƾ�Ϊ17"
			
			if(MQ_Buf > alarm_threshold){//��� Ũ�ȴ�����ֵ ����
				beep_num = 2;
				OLED_ShowCHinese((0+0),4,25);OLED_ShowCHinese((0+16),4,26);//	"�Ƽ�"

				OLED_ShowCHinese((0+64),4,55);OLED_ShowCHinese((0+80),4,56);
				sprintf((char*)string,":A");OLED_ShowStr((0+105),4,string,2);//	"�ϴ���A"
			}
			else {//��� Ũ��С����ֵ û�Ƽ�
				OLED_ShowCHinese((32+0),4,23);OLED_ShowCHinese((32+16),4,25);OLED_ShowCHinese((32+32),4,26);
																				//"û�Ƽ�"
			}
		}
		else if(JIEMIAN == 4){//����C ���º� �� 
			OLED_ShowCHinese(0,0,46);OLED_ShowCHinese((0+16),0,47);
			OLED_ShowCHinese((0+32),0,48);OLED_ShowCHinese((0+48),0,49);OLED_ShowCHinese((0+64),0,50);
			sprintf((char*)string,"10-99");OLED_ShowStr((0+80),0,string,2);//"���뱨��ֵ10-99"
			
			OLED_ShowCHinese(0,2,46);OLED_ShowCHinese((0+16),2,47);
			sprintf((char*)string,":");OLED_ShowStr((0+32),2,string,2);//		"����"
			
			OLED_ShowCHinese(0,4,51);OLED_ShowCHinese((0+16),4,52);
			sprintf((char*)string,":A");OLED_ShowStr((0+30),4,string,2);
			OLED_ShowCHinese((0+80),4,53);OLED_ShowCHinese((0+96),4,54);
			sprintf((char*)string,":B");OLED_ShowStr((0+110),4,string,2);//"ȷ����A ȡ����B"
			
			if(i == 1){sprintf((char*)string,"%d",alarm_Value[0]);OLED_ShowStr((0+48),2,string,2);}
			else if(i == 2){sprintf((char*)string,"%d",alarm_Value[1]);OLED_ShowStr((0+60),2,string,2);}
																			//�����������ֵ
		}
		else if(JIEMIAN == 5){
			OLED_ShowCHinese(0,0,58);OLED_ShowCHinese((0+16),0,59);OLED_ShowCHinese((0+32),0,60);
			OLED_ShowCHinese((0+48),0,61);OLED_ShowCHinese((0+64),0,62);OLED_ShowCHinese((0+80),0,63);
			OLED_ShowCHinese((0+96),0,64);OLED_ShowCHinese((0+112),0,65);//������Ƽ��˵ı�ʶ
			
//			OLED_ShowCHinese(0,2,46);OLED_ShowCHinese((0+16),2,47);
//			sprintf((char*)string,":");OLED_ShowStr(0,2,string,2);//����
			
			OLED_ShowCHinese(30,5,81);OLED_ShowCHinese((30+16),5,82);
			sprintf((char*)string,":A");OLED_ShowStr((30+30),5,string,2);//ȷ����A 
			OLED_ShowCHinese((0+80),5,83);OLED_ShowCHinese((0+96),5,84);
			sprintf((char*)string,":B");OLED_ShowStr((0+110),5,string,2);//�����B
		}
		else if(JIEMIAN == 6){
			OLED_ShowCHinese(0,0,90);OLED_ShowCHinese((0+16),0,91);OLED_ShowCHinese((0+32),0,92);OLED_ShowCHinese((0+48),0,93);
			OLED_ShowCHinese((0+64),0,94);OLED_ShowCHinese((0+80),0,95);//��������ʧ��
			
			OLED_ShowCHinese(0,2,96);OLED_ShowCHinese((0+16),2,97);OLED_ShowCHinese((0+32),2,98);OLED_ShowCHinese((0+48),2,99);
			OLED_ShowCHinese((0+64),2,100);
			OLED_ShowCHinese((0+0),4,101);OLED_ShowCHinese((0+16),4,102);OLED_ShowCHinese((0+32),4,103);OLED_ShowCHinese((0+48),4,104);//�������������豸
			nblot_flag = 1;
			while(nblot_flag == 1);
			OLED_CLS();
			JIEMIAN = 0;
		}
		
		ttt = Key_Scan();//����ɨ��
		
		if(ttt == 'D'){//�������D�� 
			if(JIEMIAN == 0){//��ʼ��ʱ �� ������ʼ
				JIEMIAN = 1;
				MQ_flag_Time = 5;
			}
//			else if(JIEMIAN == 1){//ֹͣ��ʱ �� ��������
//				JIEMIAN = 2;
//			}
			OLED_CLS();//ˢ��һ����Ļ
		}
		else if(ttt == '*'){//�������*�� , ������ת�ؽ���0
			OLED_CLS();
			JIEMIAN = 0;
			i = 0;j = 0;//��� ��ֹ���Ժ��������Ӱ��
			MQ_Buf =0;//��ղ���ֵ
			OLED_CLS();//ˢ��һ����Ļ
		}
		else if(ttt == 'C'){//�������C�� 
			if(JIEMIAN == 0){//����0 ���ñ���ֵ
				OLED_CLS();//ˢ��һ����Ļ
				JIEMIAN = 4;//�л�4��Ļ �� ���ñ�����ֵ
				i = 0;j = 0;//��� ��ֹ���Ժ��������Ӱ��
			}
		}	
		else if(ttt == 'A'){//�������A��
			if(JIEMIAN == 4){//����4  ���ñ�����ֵ ��A����ȷ����ť
				if(((alarm_Value[0]*10+alarm_Value[1])>=10) && ((alarm_Value[0]*10+alarm_Value[1])<100)){
					alarm_threshold = alarm_Value[0]*10+alarm_Value[1];//���õ���ֵ����Ҫ��Ļ�
					JIEMIAN = 0;//��ת������1
					OLED_CLS();//ˢ��һ����Ļ
				}
				else {//������ȷ�����趨��ֵ����ȷ �����ող����������
					alarm_Value[0] = 0;alarm_Value[1] = 0;//���
					i=0;j = 0;
					sprintf((char*)string,"");OLED_ShowStr((0+48),2,string,2);//����
					sprintf((char*)string,"");OLED_ShowStr((0+60),2,string,2);
					OLED_CLS();//ˢ��һ����Ļ
				}
			}
			else if(JIEMIAN == 3){//����ڽ���3���£�������Ҫ���Ƽ��˵ı�ʶ �ϴ�������
				if(MQ_Buf > alarm_threshold){//�ٴ�ȷ�������Ƿ��ǾƼ�
					OLED_CLS();//����
					JIEMIAN = 5;i = 0;j = 0;//ȷ���ɹ� ���л�����5
				}
			}
			
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////			
			else if(JIEMIAN == 5){//����ڽ���5���£�˵������������ �� ȷ���ϴ�
				OLED_CLS();
				
				OLED_ShowCHinese(16,2,77);OLED_ShowCHinese(32,2,78);
				OLED_ShowCHinese(48,2,79);OLED_ShowCHinese(64,2,80);//OLED��ʾ 			�����ϴ�
				memset(string2,0,sizeof(string2));
				memset(string,0,sizeof(string));
				GPS_SHUJU();
				if(gpsx.gpssta > 0){
					sprintf((char*)string,":%s:%d",Shang_Buf,MQ_Buf);
					tp=gpsx.longitude;	   
					sprintf((char *)string2,", Longitude is = %.5f %1c ",tp/=100000,gpsx.ewhemi);	//�õ������ַ���
					printf("%s	\r\n",dtbuf);
					strcat((char *)string,(char *)string2);
					tp=gpsx.latitude;	   
					sprintf((char *)string2," Latitude is = %.5f %1c \r\n",tp/=100000,gpsx.nshemi);	//�õ�γ���ַ���
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
					JIEMIAN = 6;				//�л�������6�� �� ��ʾ����ʧ�ܣ�������
				}
				
//				nblot_flag = 1;
//				while(nblot_flag == 1);
//				while(findStr(RXBuffer,(u8 *)"NSMI:SENT",200)==0)   { 
//					Usart_SendString(USART3,(char *)string);   
//					printf("Send error\r\n");
//					
//					nblot_flag = 1;			//��ʱ
//					while(nblot_flag == 1);
//					
//					if(++NBLOT_SHANG_OVERTIME == 5){//���� 50s��û�������Ϸ����� �� ���˳�
//						NBLOT_SHANG_OVERTIME = 0;
//						JIEMIAN = 6;				//�л�������6�� �� ��ʾ����ʧ�ܣ�������
//						OLED_CLS();
//						beep_num = 10;
//						break;
//					}
//				}
				if(JIEMIAN != 6){//�����û�� �������
					printf("Send ok\r\n");
					beep_num = 6;
					
					memset(Shang_Buf,0,sizeof(Shang_Buf));				
					
					JIEMIAN = 0;i = 0;j = 0;//�ϴ��ɹ� ���ؽ���0
					OLED_CLS();
				}
			}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		}
		else if(ttt == 'B'){//�������B��
			if(JIEMIAN == 4){//����ڽ���4���£�˵������Ҫ�������趨����ֵ��ֱ�ӷ���������0
				JIEMIAN = 0;
				OLED_CLS();
				alarm_Value[0] = 0;alarm_Value[1] = 0;
				i=0;
			}
			else if(JIEMIAN == 5){//����ڽ���4���£�˵������������ �Ƽ��˵ı�ʶ
				for(j = 0;j < 10;j++){//��֮ǰ�����������յ�
					sprintf((char*)string,"");OLED_ShowStr((48+(j*8)),2,string,2);
				}
				OLED_CLS();
				i = 0;j = 0;
				memset(Shang_Buf,0,sizeof(Shang_Buf));//����ϴ�������
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
		rxlen=USART1_RX_STA&0X7FFF;	//�õ����ݳ���
//		printf("aaaaaaaa%s\r\n",USART1_RX_BUF);
		for(i=0;i<rxlen;i++)TEMP_BUF[i]=USART1_RX_BUF[i];	   
		USART1_RX_STA=0;		   	//������һ�ν���
		TEMP_BUF[i]=0;				//�Զ���ӽ�����
		GPS_Analysis(&gpsx,(u8*)TEMP_BUF);//�����ַ���
		Gps_Msg_Show();
	}
//		if((lenx%500)==0);
//			LED0=!LED0
		lenx++;	
}

void GPS_INIT(void){
	u8 key = 1;
	if(Ublox_Cfg_Rate(1000,1)!=0)	//���ö�λ��Ϣ�����ٶ�Ϊ1000ms,˳���ж�GPSģ���Ƿ���λ. 
	{
		printf("NEO-6M Setting...\r\n");
		while((Ublox_Cfg_Rate(1000,1)!=0)&&key)	//�����ж�,ֱ�����Լ�鵽NEO-6M,�����ݱ���ɹ�
		{
			USART1_Init(9600);				//��ʼ������1������Ϊ9600(EEPROMû�б������ݵ�ʱ��,������Ϊ9600.)
	  		Ublox_Cfg_Prt(38400);			//��������ģ��Ĳ�����Ϊ38400
			USART1_Init(38400);				//��ʼ������1������Ϊ38400 
			Ublox_Cfg_Tp(1000000,100000,1);	//����PPSΪ1�������1��,������Ϊ100ms	    
			key=Ublox_Cfg_Cfg_Save();		//��������  
		}	  					 
		printf("NEO-6M Set Done!\r\n");
		delay_ms(500);
	}
}

//��ʾGPS��λ��Ϣ 
void Gps_Msg_Show(void)
{
 	float tp;		
	printf("\r\n\r\n\n\n\n\n");    
	tp=gpsx.longitude;	   
	sprintf((char *)dtbuf,"Longitude:%.5f %1c   \r\n",tp/=100000,gpsx.ewhemi);	//�õ������ַ���
	printf("%s	\r\n",dtbuf);
	tp=gpsx.latitude;	   
	sprintf((char *)dtbuf,"Latitude:%.5f %1c   ",tp/=100000,gpsx.nshemi);	//�õ�γ���ַ���
	printf("%s	\r\n",dtbuf);
	tp=gpsx.altitude;	   
 	sprintf((char *)dtbuf,"Altitude:%.1fm     ",tp/=10);	    			//�õ��߶��ַ���
	printf("%s	\r\n",dtbuf);
	tp=gpsx.speed;	   
 	sprintf((char *)dtbuf,"Speed:%.3fkm/h     ",tp/=1000);		    		//�õ��ٶ��ַ���	 
	printf("%s	\r\n",dtbuf);
	if(gpsx.fixmode<=3)														//��λ״̬
	{  
		sprintf((char *)dtbuf,"Fix Mode:%s",fixmode_tbl[gpsx.fixmode]);	
		printf("%s	\r\n",dtbuf);
	}	 	   
	sprintf((char *)dtbuf,"Valid satellite:%02d",gpsx.posslnum);	 		//���ڶ�λ��������
	printf("%s	\r\n",dtbuf);
	sprintf((char *)dtbuf,"Visible satellite:%02d",gpsx.svnum%100);	 		//�ɼ�������
	printf("%s	\r\n",dtbuf);
	printf("%s	\r\n",dtbuf);	
	sprintf((char *)dtbuf,"UTC Date:%04d/%02d/%02d   ",gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//��ʾUTC����
	printf("year2:%d\r\n",gpsx.utc.year);
	printf("%s	\r\n",dtbuf);	
	sprintf((char *)dtbuf,"UTC Time:%02d:%02d:%02d   ",gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//��ʾUTCʱ��
	printf("%s	\r\n",dtbuf);
}

void StringToHex(char *str, unsigned char *strhex){
	unsigned char  i, cnt = 0;
	char *p = str;             //ֱ��p��ʼ��Ϊָ��str
	unsigned char len = strlen(str); //��ȡ�ַ����е��ַ�����

	while (*p != '\0') {        //�������ж�
		for (i = 0; i < len; i++)  //ѭ���жϵ�ǰ�ַ������ֻ���Сд�ַ����Ǵ�д��ĸ
		{
			if ((*p >= '0') && (*p <= '9')) //��ǰ�ַ�Ϊ����0~9ʱ
				strhex[cnt] = *p - '0' + 0x30;//תΪʮ������

			if ((*p >= 'A') && (*p <= 'Z')) //��ǰ�ַ�Ϊ��д��ĸA~Zʱ
				strhex[cnt] = *p - 'A' + 0x41;//תΪʮ������

			if ((*p >= 'a') && (*p <= 'z')) //��ǰ�ַ�ΪСд��ĸa~zʱ
				strhex[cnt] = *p - 'a' + 0x61;  //תΪʮ������
			if (*p == ':') //��ǰ�ַ�ΪСд��ĸa~zʱ
				strhex[cnt] = 0x3A;  //תΪʮ������
			if (*p == ' ') //��ǰ�ַ�ΪСд��ĸa~zʱ
				strhex[cnt] = 0x20;  //תΪʮ������
			if (*p == '=') //��ǰ�ַ�ΪСд��ĸa~zʱ
				strhex[cnt] = 0x3D;  //תΪʮ������
			if (*p == ',') //��ǰ�ַ�ΪСд��ĸa~zʱ
				strhex[cnt] = 0x2C;  //תΪʮ������
			if (*p == '.') //��ǰ�ַ�ΪСд��ĸa~zʱ
				strhex[cnt] = 0x2E;  //תΪʮ������
			p++;    //ָ����һ���ַ�
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

