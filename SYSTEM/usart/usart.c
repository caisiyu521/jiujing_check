#include "sys.h"
#include "usart.h"	  
#include "string.h"	
////////////////////////////////////////////////////////////////////////////////// 	 

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE { 
	int handle; 
}; 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) { 
	x = x; 
} 

//USART1    重定义fputc函数 
int fputc(int ch, FILE *f){      
	while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
    USART2->DR = (u8) ch;      
	return ch;
}
#endif 

 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
//u8 USART1_RX_BUF[USART1_MAX_RECV_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART2_RX_BUF[USART_REC_LEN];
u8 USART3_RX_BUF[USART_REC_LEN];
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
//u16 USART1_RX_STA=0;       //接收状态标记	  
u16 USART2_RX_STA=0;       //接收状态标记
u16 USART3_RX_STA=0; 
u16 len ,t;

void uart2_init(u32 bound){
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//Tx 脚 复用推挽 输出
	GPIO_InitStructure.GPIO_Pin = USART2_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//Rx 脚 浮空 输入
	GPIO_InitStructure.GPIO_Pin = USART2_RxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	
	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口1
	
	//打开 接收中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	// 使能串口2
	USART_Cmd(USART2, ENABLE);
}

void uart3_init(u32 bound){
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.9

	//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.10  
	
	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART3, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
	USART_Cmd(USART3, ENABLE);                    //使能串口1 
}

#include "NetWork.h"
//extern u8 nblot_flag ;
extern u8 RXBuffer[RXBUFFER_LEN];

void USART3_IRQHandler(void)                	//串口1中断服务程序
{
	static u8 i = 0 ;
	
    if(USART_GetITStatus(USART3, USART_IT_RXNE)){ //判断接收数据寄存器是否有数据
        RXBuffer[i++] = USART_ReceiveData(USART3);
        if(i==RXBUFFER_LEN)                      {
            i = RXBUFFER_LEN-1;
        }
    }
    if(USART_GetITStatus(USART3, USART_IT_IDLE)){
        USART_ReceiveData(USART3);              //读一次UART可以清除空闲标志位
		printf("%s\r\n",RXBuffer);
        i = 0;
	}   		 
}



//#include "NetWork.h"
////extern u8 nblot_flag ;
//extern u8 RXBuffer[RXBUFFER_LEN];
////extern int Str16_To_Hex(char * buf , char countnum);
////uint8_t NBLOT_BUFFER_Hex[100];

//void USART3_IRQHandler(void)                	//串口1中断服务程序
//{
//	static u8 i = 0 ;
//	
//    if(USART_GetITStatus(USART3, USART_IT_RXNE)) //判断接收数据寄存器是否有数据
//    {
//        RXBuffer[i++] = USART_ReceiveData(USART3);
//        if(i==RXBUFFER_LEN)                     //超出接收缓冲范围,可能的情形是ESP8266复位,为防止溢出必须设置索引                  
//        {
//            i = RXBUFFER_LEN-1;
//        }
//    }
//    if(USART_GetITStatus(USART3, USART_IT_IDLE)){
//		
//        USART_ReceiveData(USART3);              //读一次UART可以清除空闲标志位
//		printf("%s\r\n",RXBuffer);
////		if(strstr((const char *)RXBuffer,"8,010300000001840A")){
////			printf("LED!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
////			Str16_To_Hex("010300000001840A",)
////		}
////		if(strstr((const char *)RXBuffer,"8,010300010001D5CA")){
////			printf("LED****************************\r\n");
////		}
//		
////		if(strstr("8,0103" ,(const char *) RXBuffer)){
////			printf("LED!!!!\r\n");	
////		}
//        i = 0;
//	}   		 
//}

//uint16_t GenerateCrc(uint8_t *c,int num)
//{
//	uint16_t crc = 0xffff,code = 0xa001;
//	char i,j;
//	for(i = 0;i< num;i++){
//		crc ^= c[i]; 
//		for(j = 0;j < 8;j++){
//			if(crc&1){
//				crc>>=1;
//				crc^=code;
//			}else{
//				crc>>=1;
//			}
//		}
//	}	
//	return crc;
//}

//void USART1_IRQHandler(void)                	//串口1中断服务程序
//{
//	u8 Res;
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//	{
//		Res =USART_ReceiveData(USART1);	//读取接收到的数据
//		if((USART1_RX_STA&0x8000)==0){//接收未完成
//			if(USART1_RX_STA&0x4000){//接收到了0x0d
//				if(Res!=0x0a)USART1_RX_STA=0;//接收错误,重新开始
//				else USART1_RX_STA|=0x8000;	//接收完成了 
//			}
//			else {//还没收到0X0D	
//				if(Res==0x0d)USART1_RX_STA|=0x4000;
//				else{
//					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
//					USART1_RX_STA++;
//					if(USART1_RX_STA>(USART_REC_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收	  
//				}	
//			}				
//		}
//	}   		 
//}

void USART2_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART2);	//读取接收到的数据
		
		if((USART2_RX_STA&0x8000)==0){//接收未完成
			if(USART2_RX_STA&0x4000){//接收到了0x0d
				if(Res!=0x0a)USART2_RX_STA=0;//接收错误,重新开始
				else USART2_RX_STA|=0x8000;	//接收完成了 
			}
			else {//还没收到0X0D	
				if(Res==0x0d)USART2_RX_STA|=0x4000;
				else{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
					USART2_RX_STA++;
					if(USART2_RX_STA>(USART_REC_LEN-1))USART2_RX_STA=0;//接收数据错误,重新开始接收	  
				}	
			}				
		}
	}   		 
} 


////发送一个字节
//void Usart_SendChar(USART_TypeDef * pUSARTx,  uint8_t ch){
//	/* 发送一个字节数据到 USART */
//	USART_SendData (pUSARTx ,ch);
//	/* 等待发送数据寄存器为空 */
//	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
//}
////发送一个字符串
//void Usart_SendString(USART_TypeDef * pUSARTx, char *str){
//	unsigned int k=0;
//	do {
//		Usart_SendChar( pUSARTx, *(str + k) );
//		k++;
//	} while (*(str + k)!='\0');
//}

void sendByte(USART_TypeDef *USARTx, u16 byte)
{
	USART_ClearFlag(USARTx, USART_FLAG_TC);             //软件清除发送完成标志位
    USART_SendData(USARTx, byte);                       //发送一个字节
    while (!USART_GetFlagStatus(USARTx, USART_FLAG_TC));//等待发送完成
    USART_ClearFlag(USARTx, USART_FLAG_TC);             //软件清除发送完成标志位
}

/**
 * 功能：指定某个串口发送字符串
 * 参数：USARTx：使用的目标串口x为1-3
 *       str：字符串指针
 * 返回值：None
 */
void sendString(USART_TypeDef *USARTx, char *str)
{
    while (*str)
    {
        sendByte(USARTx,*str++);
    }
}

//发送一个字节
void Usart_SendChar(USART_TypeDef * pUSARTx,  uint8_t ch){
	/* 发送一个字节数据到 USART */
	USART_SendData (pUSARTx ,ch);
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}
//发送一个字符串
void Usart_SendString(USART_TypeDef * pUSARTx,char *str){
	
	unsigned int k=0;
	printf("%s\r\n",str);
	do {
		Usart_SendChar( pUSARTx, *(str + k) );
		k++;
	} while (*(str + k)!='\0');
	  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
	  {}
}


  
#endif	

//		//主函数     串口1 接受函数
//		if(USART1_RX_STA&0x8000){					   
//			printf("%s \r\n" , USART1_RX_BUF);
//			USART1_RX_STA=0;
//			memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF));
//		}

//void USART1_IRQHandler(void)                	//串口1中断服务程序
//{
//	u8 Res;
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//	{
//		Res =USART_ReceiveData(USART1);	//读取接收到的数据
////		printf("%c\r\n",Res);
//		if((USART1_RX_STA&0x8000)==0){//接收未完成
//			if(USART1_RX_STA&0x4000){//接收到了0x0d
//				if(Res!=0x0a)USART1_RX_STA=0;//接收错误,重新开始
//				else USART1_RX_STA|=0x8000;	//接收完成了 
//			}
//			else {//还没收到0X0D	
//				if(Res==0x0d)USART1_RX_STA|=0x4000;
//				else{
//					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
//					USART1_RX_STA++;
//					if(USART1_RX_STA>(USART_REC_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收	  
//				}	
//			}				
//		}
//	}   		 
//}
//void USART2_IRQHandler(void)                	//串口1中断服务程序
//{
//	u8 Res;
//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//	{
//		Res =USART_ReceiveData(USART2);	//读取接收到的数据
//		
//		if((USART2_RX_STA&0x8000)==0){//接收未完成
//			if(USART2_RX_STA&0x4000){//接收到了0x0d
//				if(Res!=0x0a)USART2_RX_STA=0;//接收错误,重新开始
//				else USART2_RX_STA|=0x8000;	//接收完成了 
//			}
//			else //还没收到0X0D{	
//				if(Res==0x0d)USART2_RX_STA|=0x4000;
//				else{
//					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
//					USART2_RX_STA++;
//					if(USART2_RX_STA>(USART_REC_LEN-1))USART2_RX_STA=0;//接收数据错误,重新开始接收	  
//				}		 
//			}
//		}   		 
//} 
////以其他字符结束  
//void USART1_IRQHandler(void)                	//串口1中断服务程序
//{
//	u8 Res;
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//	{
//		Res =USART_ReceiveData(USART1);	//读取接收到的数据
//		if((USART1_RX_STA&0x8000)==0){//接收未完成
//			if(Res==0x3b)USART1_RX_STA|=0x8000;
//			else{
//				USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
//				USART1_RX_STA++;
//				if(USART1_RX_STA>(USART_REC_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收	  
//			}			
//		}
//	}   		 
//} 

////	串口1 接受函数
//		if(USART1_RX_STA&0x8000){					   
//			len=USART1_RX_STA&0x3fff;//得到此次接收到的数据长度
//			for(t=0;t<len;t++){
//				USART_SendData(USART1, USART1_RX_BUF[t]);//向串口1发送数据
//				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
//			}
//			USART1_RX_STA=0;
//		}

////	串口2 接受函数
//		if(USART2_RX_STA&0x8000){					   
//			len=USART2_RX_STA&0x3fff;//得到此次接收到的数据长度
//			for(t=0;t<len;t++){
//				USART_SendData(USART2, USART2_RX_BUF[t]);//向串口1发送数据
//				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
//			}
//			USART2_RX_STA=0;
//			Usart_SendString(USART2,"\r\n");
//		}

