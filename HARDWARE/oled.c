#include "oled.h"

#include "delay.h"
#include "codetab.h"

/**
**  ��ʼ��IIC�ӿ�
**/
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructer;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructer.GPIO_Pin=GPIO_Pin_11 | GPIO_Pin_12; //6--SCL   7--SDA
    GPIO_InitStructer.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructer.GPIO_Mode=GPIO_Mode_Out_PP;

    GPIO_Init(GPIOA, &GPIO_InitStructer);
}

/**
**  ����SDAΪ���
**/
void SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructer;
    GPIO_InitStructer.GPIO_Pin= GPIO_Pin_12;
    GPIO_InitStructer.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructer.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructer);
}


/**
**  ����SDAΪ����
**/
void SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructer;
    GPIO_InitStructer.GPIO_Pin= GPIO_Pin_12;
    GPIO_InitStructer.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructer.GPIO_Mode=GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructer);
}

//��ʼ�ź�
void IIC_Start(void)
{
    SDA_OUT();
    IIC_SDA=1;
    IIC_SCL=1;
    delay_us(2);
    IIC_SDA=0;
    delay_us(2);
    IIC_SCL=0;
    delay_us(2);
}

void IIC_Stop(void)
{
    IIC_SCL=1;
    IIC_SDA=0;
    delay_us(2);
    IIC_SDA=1;
    delay_us(2);
}


/*
*   ����1--Ӧ�����
*   �Ż�0--Ӧ����ȷ
*/
u8 IIC_Wait_Ask(void)
{
    int count=0;
    SDA_IN();
    IIC_SCL=1;
    delay_us(2);
    while(READ_SDA)
    {
        count++;
        if(count>250)
        {
            IIC_Stop();
            return 1;
        }   
    }
    IIC_SCL=0;
    delay_us(2);
    return 0;
}

//дһ���ֽ�
void IIC_WriteByte(u8 data)
{
    u8 i;
    SDA_OUT();
    for(i=0;i<8;i++)
    {
        IIC_SCL=0;
        delay_us(2);
        if(data & 0x80)     //MSB,�Ӹ�λ��ʼһλһλ����
            IIC_SDA=1;
        else
            IIC_SDA=0;
        IIC_SCL=1;
        delay_us(2);
        IIC_SCL=0;
        data<<=1;

    }
}


u8 IIC_ReadByte(void)
{
    u8 data,i;
    IIC_SDA=1;
    delay_us(2);
    for(i=0;i<8;i++)
    {
        data<<=1;
        IIC_SCL=0;
        delay_us(2);
        IIC_SCL=1;
        delay_us(2);
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12))
            data=data | 0x01;
        else 
            data=data & 0xFE;

    }
    IIC_SCL=0;
    delay_us(2);
    return data;

}


void WriteCmd(u8 command)
{
    IIC_Start();
    IIC_WriteByte(0x78);//OLED��ַ
    IIC_Wait_Ask();
    IIC_WriteByte(0x00);//�Ĵ�����ַ
    IIC_Wait_Ask();
    IIC_WriteByte(command);
    IIC_Wait_Ask();
    IIC_Stop();
}


void WriteDat(u8 data)
{
    IIC_Start();
    IIC_WriteByte(0x78);//OLED��ַ
    IIC_Wait_Ask();
    IIC_WriteByte(0x40);//�Ĵ�����ַ
    IIC_Wait_Ask();
    IIC_WriteByte(data);
    IIC_Wait_Ask();
    IIC_Stop();
}

void OLED_Init(void)
{
    delay_ms(100); //�������ʱ����Ҫ

    WriteCmd(0xAE); //display off
    WriteCmd(0x20); //Set Memory Addressing Mode    
    WriteCmd(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    WriteCmd(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
    WriteCmd(0xc8); //Set COM Output Scan Direction
    WriteCmd(0x00); //---set low column address
    WriteCmd(0x10); //---set high column address
    WriteCmd(0x40); //--set start line address
    WriteCmd(0x81); //--set contrast control register
    WriteCmd(0xff); //���ȵ��� 0x00~0xff
    WriteCmd(0xa1); //--set segment re-map 0 to 127
    WriteCmd(0xa6); //--set normal display
    WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
    WriteCmd(0x3F); //
    WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    WriteCmd(0xd3); //-set display offset
    WriteCmd(0x00); //-not offset
    WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
    WriteCmd(0xf0); //--set divide ratio
    WriteCmd(0xd9); //--set pre-charge period
    WriteCmd(0x22); //
    WriteCmd(0xda); //--set com pins hardware configuration
    WriteCmd(0x12);
    WriteCmd(0xdb); //--set vcomh
    WriteCmd(0x20); //0x20,0.77xVcc
    WriteCmd(0x8d); //--set DC-DC enable
    WriteCmd(0x14); //
    WriteCmd(0xaf); //--turn on oled panel
}


 /**
  * @brief  OLED_ON����OLED�������л���
  * @param  ��
    * @retval ��
  */
void OLED_ON(void)
{
    WriteCmd(0X8D);  //���õ�ɱ�
    WriteCmd(0X14);  //������ɱ�
    WriteCmd(0XAF);  //OLED����
}


 /**
  * @brief  OLED_SetPos�����ù��
  * @param  x,���xλ��
    *                   y�����yλ��
  * @retval ��
  */
void OLED_SetPos(unsigned char x, unsigned char y) //������ʼ������
{ 
    WriteCmd(0xb0+y);
    WriteCmd(((x&0xf0)>>4)|0x10);
    WriteCmd((x&0x0f)|0x01);
}

 /**
  * @brief  OLED_Fill�����������Ļ
  * @param  fill_Data:Ҫ��������
    * @retval ��
  */
void OLED_Fill(unsigned char fill_Data)//ȫ�����
{
    unsigned char m,n;
    for(m=0;m<8;m++)
    {
        WriteCmd(0xb0+m);       //page0-page1
        WriteCmd(0x00);     //low column start address
        WriteCmd(0x10);     //high column start address
        for(n=0;n<128;n++)
            {
                WriteDat(fill_Data);
            }
    }
}

void OLED_CLS(void)//����
{
    OLED_Fill(0x00);
}

/**
  * @brief  OLED_ShowStr����ʾcodetab.h�е�ASCII�ַ�,��6*8��8*16��ѡ��
  * @param  x,y : ��ʼ������(x:0~127, y:0~7);
    *                   ch[] :- Ҫ��ʾ���ַ���; 
    *                   TextSize : �ַ���С(1:6*8 ; 2:8*16)
    * @retval ��
  */
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
    unsigned char c = 0,i = 0,j = 0;
    switch(TextSize)
    {
        case 1:
        {
            while(ch[j] != '\0')
            {
                c = ch[j] - 32;
                if(x > 126)
                {
                    x = 0;
                    y++;
                }
                OLED_SetPos(x,y);
                for(i=0;i<6;i++)
                    WriteDat(F6x8[c][i]);
                x += 6;
                j++;
            }
        }break;
        case 2:
        {
            while(ch[j] != '\0')
            {
                c = ch[j] - 32;
                if(x > 120)
                {
                    x = 0;
                    y++;
                }
                OLED_SetPos(x,y);
                for(i=0;i<8;i++)
                    WriteDat(F8X16[c*16+i]);
                OLED_SetPos(x,y+1);
                for(i=0;i<8;i++)
                    WriteDat(F8X16[c*16+i+8]);
                x += 8;
                j++;
            }
        }break;
    }
}


void OLED_ShowCHinese(u8 x,u8 y,u8 no)
{      			    
	u8 t,adder=0;
	OLED_SetPos(x,y);	
    for(t=0;t<16;t++){
		WriteDat(Chinese[2*no][t]);
		adder+=1;
    }	
	OLED_SetPos(x,y+1);	
    for(t=0;t<16;t++){	
		WriteDat(Chinese[2*no+1][t]);
		adder+=1;
    }					
}

