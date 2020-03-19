#include "network.h"
#include "led.h"
#include "usart.h"
#include "delay.h"
#include "string.h"

uchar  GPRS_Delay_1S = 0;

//unsigned char *SMS_Num="\"+8613800377500\"";//�������ĺ� �������ĺŵĽ��ܿ��Կ����ڵ��Աʼǵĵڶ�����
//unsigned char *phone="\"15517175421\"";     //���Ž��շ��ֻ���
//unsigned char *content="Thanks";//���Ͷ�������
const char *string123 = "AT+CIPSTART=\"TCP\",\"103.46.128.43\",56695\r\n";	//IP��¼������

u8 TXBuffer[TXBUFFER_LEN] = {0};  //����ͨ�����ݷ��ͻ���
u8 RXBuffer[RXBUFFER_LEN] = {0};  //����ͨ�����ݽ��ջ���
uchar OVER = 0;	

u8 initGPRS(void){
		//��ֹһֱ��ѭ��
	Usart_SendString(USART3,"AT\r\n");     //����GPRSģ���뵥Ƭ���Ƿ�����
	if(findStr(RXBuffer,(u8 *)"OK",10)==0)   { 
		Usart_SendString(USART3,"AT\r\n"); 
		GPRS_Delay_1S = 1;while(GPRS_Delay_1S);				//��ȷ��ʱ1S
		if(findStr(RXBuffer,(u8 *)"OK",40)==0)   {
			printf("AT����ָ�����\r\n");
			return 0;  
		}     
    }
	memset(RXBuffer,0,sizeof(RXBuffer));
	
	Usart_SendString(USART3,"AT+CREG?\r\n");   		//��ѯģ���Ƿ�ע��ɹ�		�Ƚ���Ҫ
	GPRS_Delay_1S = 1;while(GPRS_Delay_1S);			//��ȷ��ʱ1S
	OVER  = 0;
	while(findStr(RXBuffer,(u8 *)"+CREG: 0,1",40)==0){ //���û���յ����ͳɹ��Ļظ����������
		Usart_SendString(USART3,"AT+CREG?\r\n");	//��ȷ��ʱ1S
		printf("Send error\r\n");
		if(++OVER == 20)	return 0;				//����10�뻹û�����ӳɹ� ���˳�
    }
	memset(RXBuffer,0,sizeof(RXBuffer));
	printf("zhuce success ok\r\n");
	
    return 1;                         //���óɹ�
}

u8 Set_Text_Mode(void)
{
	Usart_SendString(USART3,"ATE0\r\n");			//�رջ���
	if(findStr(RXBuffer,(unsigned char*)"OK",40)==0) {
        return 0;      
    }
	
	memset(RXBuffer,0,sizeof(RXBuffer));
	Usart_SendString(USART3,"AT+CIPSHUT\r\n");		//�ر��ƶ�����		
	if(findStr(RXBuffer,(unsigned char*)"OK",40)==0) {
        return 0;      
    }
	
	memset(RXBuffer,0,sizeof(RXBuffer));
	Usart_SendString(USART3,"AT+CGCLASS=\"B\"\r\n");//����GPRS�ƶ�̨���ΪB,֧�ְ����������ݽ��� 
	GPRS_Delay_1S = 1;while(GPRS_Delay_1S);
	if(findStr(RXBuffer,(unsigned char*)"OK",50)==0) {
        return 0;      
    }
	
	memset(RXBuffer,0,sizeof(RXBuffer));
	Usart_SendString(USART3,"AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");//����PDP������,��������Э��,��������Ϣ
	if(findStr(RXBuffer,(unsigned char*)"OK",40)==0) {
        return 0;      
    }
	
	memset(RXBuffer,0,sizeof(RXBuffer));
	Usart_SendString(USART3,"AT+CGATT=1\r\n");	//����GPRSҵ��
	if(findStr(RXBuffer,(unsigned char*)"OK",40)==0) {
        return 0;      
    }
	
	memset(RXBuffer,0,sizeof(RXBuffer));
	Usart_SendString(USART3,"AT+CIPCSGP=1,\"CMNET\"\r\n");	//����ΪGPRS����ģʽ
	if(findStr(RXBuffer,(unsigned char*)"OK",40)==0) {
        return 0;      
    }
	
	
	
	memset(RXBuffer,0,sizeof(RXBuffer));
	Usart_SendString(USART3,"AT+CIPHEAD=1\r\n");	//���ý���������ʾIPͷ(�����ж�������Դ,���ڵ�·������Ч)
	if(findStr(RXBuffer,(unsigned char*)"OK",40)==0) {
        return 0;      
    }
	
	memset(RXBuffer,0,sizeof(RXBuffer));
	Usart_SendString(USART3,(char*)string123);				//����ΪGPRS����ģʽ
	while(findStr(RXBuffer,(u8 *)"OK",40)==0){ //���û���յ����ͳɹ��Ļظ����������
		printf("Send error\r\n");
		Usart_SendString(USART3,(char*)string123);				//����ΪGPRS����ģʽ
		if(++OVER == 5)	return 0;				//����10�뻹û�����ӳɹ� ���˳�
    }
	
	OVER = 0;
	while(findStr(RXBuffer,(u8 *)"CONNECT OK",40)==0){ //���û���յ����ͳɹ��Ļظ����������
		printf("Send error\r\n");
		if(++OVER == 5)	return 0;				//����10�뻹û�����ӳɹ� ���˳�
    }
	return 1;   
}

u8 GPRS_Send_String(uchar *str){
	Usart_SendString(USART3,"AT+CIPSEND\r\n");
	GPRS_Delay_1S = 1;while(GPRS_Delay_1S);
	GPRS_Delay_1S = 1;while(GPRS_Delay_1S);
	
	Usart_SendString(USART3,(char *)str);	//����ΪGPRS����ģʽ
	GPRS_Delay_1S = 1;while(GPRS_Delay_1S);
	GPRS_Delay_1S = 1;while(GPRS_Delay_1S);		//��ȷ��ʱ1S
	USART_SendData(USART3,0X1A);	//����ΪGPRS����ģʽ
	OVER = 0;
	while(findStr(RXBuffer,(u8 *)"SEND OK",10)==0){ //���û���յ����ͳɹ��Ļظ����������
		printf("Send error\r\n");
		GPRS_Delay_1S = 1;while(GPRS_Delay_1S);		//��ȷ��ʱ1S
		if(++OVER == 10)	return 0;				//����10�뻹û�����ӳɹ� ���˳�
    }
	return 1;
}

//u8 Set_Text_Mode(void)
//{
//	char temp[50]="AT+CSCA=";							
//	Usart_SendString(USART3,"ATE0\r\n");				//�رջ���
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

//	strcat(temp,(char*)SMS_Num);				//�������ĺ�
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
//	strcat(temp,(char*)phone); //�ַ���ƴ�Ӻ������⺯����
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
//	USART_SendData(USART3 ,0X1A);  //���ͽ�����
////	Usart_SendString(USART3,"\r\n");
//	
//	OVER  = 0;
////	
//	while(findStr(RXBuffer,(u8 *)"+CMGS",200)==0){ //���û���յ����ͳɹ��Ļظ����������
//		printf("Send error\r\n");
//		GPRS_Delay_1S = 1;while(GPRS_Delay_1S);		//��ȷ��ʱ1S
//		GPRS_Delay_1S = 1;while(GPRS_Delay_1S);		//��ȷ��ʱ1S
//		if(++OVER == 10)	
//			return 0;				//����10�뻹û�����ӳɹ� ���˳�
//    }
//	memset(RXBuffer,0,sizeof(RXBuffer)); 
//	return 1;
//}


u8 findStr(u8* dest,u8* src,u16 retry_cn)
{
	u16 retry = retry_cn;                   //��ʱʱ��
	u8 result_flag = 0;                     //���ҽ��
	
	while(strstr((const char *)dest,(const char *)src)==0 && --retry!=0){//�ȴ����ڽ�����ϻ�ʱ�˳�
		GPRS_Delay_1S = 1;while(GPRS_Delay_1S);
	}
	if(retry==0){                     //�����ʱ��������,��ʱ����0
	   return 0;
	}
	result_flag = 1;                        //ִ�е�����˵��һ������, ��ʾ���ҳɹ�

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
//u8 TXBuffer[TXBUFFER_LEN] = {0};  //����ͨ�����ݷ��ͻ���
//u8 RXBuffer[RXBUFFER_LEN] = {0};  //����ͨ�����ݽ��ջ���
//u8 nblot_Buf[50];
//extern u8 NBLOT_SHANG_OVERTIME;

//u8 findStr(u8* dest,u8* src,u16 retry_cn)
//{
//	u16 retry = retry_cn;                   //��ʱʱ��
//	u8 result_flag = 0;                     //���ҽ��
//	
//	while(strstr((const char *)dest,(const char *)src)==0 && --retry!=0){//�ȴ����ڽ�����ϻ�ʱ�˳�
//		Delay_ms(10);
//	}

//	if(retry==0){                     //�����ʱ��������,��ʱ����0
//	   return 0;
//	}
//	result_flag = 1;                        //ִ�е�����˵��һ������, ��ʾ���ҳɹ�

//	if(result_flag){
//		return 1;
//	}else {
//		return 0;
//	}
//}

///**
// * ���ܣ���ʼ��ESP8266
// * ������None
// * ����ֵ����ʼ���������0Ϊ��ʼ���ɹ�,0Ϊʧ��
// */

//u8 initESP8266(void){
//	Usart_SendString(USART3,"AT\r\n");    delay_ms(100);  //����nblotģ���뵥Ƭ���Ƿ�����
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
//	Usart_SendString(USART3,"AT+CGSN=1\r\n");   delay_ms(100);//���nblot�Ƿ��Ѿ��忨
//	if(findStr(RXBuffer,(u8 *)"OK",200)==0)   { 
//		printf("read card error\r\n");
//        return 0;      
//    }
//	memset(RXBuffer,0,sizeof(RXBuffer));
//	printf("read card ok\r\n");
//	
//	Usart_SendString(USART3,"AT+NCDP=117.60.157.137,5683\r\n");    delay_ms(100); //��nblot��coapЭ������͸���Ʒ�����
//	if(findStr(RXBuffer,(u8 *)"OK",200)==0){ 
//		printf("AT+NCDP error\r\n");
//        return 0;      
//    }
//	printf("ip config ok\r\n");
//	
//	memset(RXBuffer,0,sizeof(RXBuffer));
//	Usart_SendString(USART3,"AT+NRB\r\n");//����nblotģ��
//	nblot_flag = 1;
//	while(nblot_flag == 1);
//	nblot_flag = 1;
//	while(nblot_flag == 1);
//	printf("chongqi config ok\r\n");
//	
//	memset(RXBuffer,0,sizeof(RXBuffer));
//	
//	Usart_SendString(USART3,"AT+NSMI=1\r\n");   delay_ms(100);//ʹ����Ϣ����֪ͨ
//	if(findStr(RXBuffer,(u8 *)"OK",200)==0)   { 
//		printf("NSMI card error\r\n");
//        return 0;      
//    }
//	printf("NSMI config ok\r\n");
//	
//	Usart_SendString(USART3,"AT+NNMI=2\r\n");   delay_ms(100);//ʹ����Ϣ����֪ͨ
//	if(findStr(RXBuffer,(u8 *)"OK",200)==0)   { 
//		printf("NNMI card error\r\n");
//        return 0;      
//    }
//	printf("NNMI config ok\r\n");
////	
//	Usart_SendString(USART3,"AT+NMGS=4,496E6974\r\n");//��һ����Ϣ��¼͸���ƣ����͸�����յ�һ����Ϣ��ʼ�����豸��
//	nblot_flag = 1;//��ʱ5���ӣ���ʱ����ʱ�Ƚ�׼ȷ
//	while(nblot_flag == 1);
//	while(findStr(RXBuffer,(u8 *)"NSMI:SENT",200)==0)   { //���û���յ����ͳɹ��Ļظ����������
//		Usart_SendString(USART3,"AT+NMGS=4,496E6974\r\n");
//		printf("Send error\r\n");
//		nblot_flag = 1;			//��ʱ5����
//		while(nblot_flag == 1);
//		if(++NBLOT_SHANG_OVERTIME == 10){//���� 50s��û�������Ϸ����� �� ���˳�
//			NBLOT_SHANG_OVERTIME = 0;
//			return 0;
//		}
//    }
//	
////	AT+NMGR
//	printf("Send  ok\r\n");
//	memset(RXBuffer,0,sizeof(RXBuffer));
//    return 1;                         //���óɹ�
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


