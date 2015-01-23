/*
 * MP3ģ����Գ���
 * 
 * ��;��MP3ģ����Գ���
 *       vs1003 ��Ӳ�����Գ�������оƬΪSTC12LE5A60S2
 *       ������΢������(��SPI�ӿڵ�)ֻ���Լ��޸ļ�������
 *       ���ڲ���ӲSPI�ӿڵ�΢������������IO����SPI��ʱ��ģ��
 * 
 * ����					����				��ע
 * Huafeng Lin			20010/09/10			����
 * Huafeng Lin			20010/09/10			�޸�
 * 
 */
 
#include "vs1003.h"
//#include "MusicDataMP3.c"																			---------change----------
#include <avr/io.h>
#include "macr.h"
//#include "oled.h"
//#include "sdcard.c"
#include "sdcard.h"
#ifndef F_CPU
#define F_CPU 4000000
#endif
#include <util/delay.h>
#include "nRF24L01.h"
#include "nRF24L01_Reg.h"
//#include <intrins.h>

#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long
#define bool bit
#define true 1
#define flase 0
 volatile ulong sector1=0;
void Mp3PutInReset()  { MP3_XRESET = 0; }
void Mp3ReleaseFromReset()  { MP3_XRESET =1; }
void Mp3SelectControl()  { MP3_XCS = 0; }
void Mp3DeselectControl()  { MP3_XCS = 1; }
void Mp3SelectData()		{ MP3_XDCS = 0; }
void Mp3DeselectData()	{ MP3_XDCS = 1; }

#define Storage 32768//8g
//unsigned char lux[5]={32,32,32,32,32};
unsigned char lux[512]={1,1,1,1,1,1,1,1,1,11,1,1,1,1,1,1,1,1,1,11,1,1,1,1,1,1,1,11,1,1};
unsigned char tar[32]={32,32,32,32,32,32,32};
unsigned char save[32]={0};
volatile unsigned int value;
unsigned int high=0xffff,low=0xaaaa,delay_a=0;
 long b,a;

//���SD����д���������
#define uint8 unsigned char

void LCD_write_english_string(unsigned char X,unsigned char Y,char *s);

//#define SPIWait()	{ while((S0SPSR & 0x80) == 0); }//�ȴ�SPI�����ݷ������

//#define SPI_RESULT_BYTE  S0SPDR

//extern long volatile timeval; //������ʱ��ȫ�ֱ���
//1ms Delayfunction
//void Delay(uchar ucDelayCount)
void wait(uchar ucDelayCount)
{
	uchar ucTempCount;
	uchar uci;

	for(ucTempCount=0; ucTempCount<ucDelayCount; ucTempCount++)
	{
//		uci = 200;	//Err
//		uci = 250;	//OK
		uci = 230;
		while(uci--)
		{
		asm("NOP");
	   	}
	}
}

//#define wait(x) Delay(x)
/**********************************************************/
/*  �������� :   MSPI_Init                                */
/*  �������� �� ��ʼ��SPI�ӿڣ�����Ϊ������               */
/*  ����     :  ��                                        */
/*  ����ֵ   :  ��                                        */
/*--------------------------------------------------------*/
void  MSPI_Init(void)
{  
/*
	PINSEL0 = (PINSEL0 & 0xFFFF00FF) | 0x00005500;	//ѡ�� SPI 
        S0SPCCR = 0x08;		                        // SPI ʱ������
 	S0SPCR  = (0 << 3) |				// CPHA = 0, 
 		  (0 << 4) |				// CPOL = 0, 
 		  (1 << 5) |				// MSTR = 1, 
 		  (0 << 6) |				// LSBF = 0, 
 	          (0 << 7);				// SPIE = 0, 
*/
//	DDRA=0xff;
//	DDR_PA2=0;/////////dreqΪ����                              ---------------change----------------
//	DDR_PA5=0;/////////soΪ����
	DDR_PA7=0;
//	c_SPI_SO = 1;
//	MP3_DREQ = 1;

}

/**********************************************************/
/*  �������� :  InitPortVS1003                            */
/*  �������� �� MCU��vs1003�ӿڵĳ�ʼ��                   */
/*  ����     :  ��                                        */
/*  ����ֵ   :  ��                                        */
/*--------------------------------------------------------*/
void  InitPortVS1003(void)
{
	MSPI_Init();//SPI�ڵĳ�ʼ��
//	IODIR &= 0xfffeffff;   //�����ӿ��ߵ����ã�����dreq Ϊ�����
//	IODIR |= MP3_XRESET | MP3_XCS | MP3_XDCS;//xRESET��xCS��xDS��Ϊ�����
//	IOSET |= MP3_XRESET | MP3_XCS | MP3_XDCS;//xRESET��xCS��xDSĬ������ߵ�ƽ	
//	MP3_DREQ = 1;		//��Ϊ����

//	MP3_XRESET = 1;
	MP3_XCS = 1;
	MP3_XDCS = 0;
}

//uint8 SD_SPI_ReadByte(void);
//void SD_SPI_WriteByte(uint8 ucSendData);

//#define SPI_RecByte()  SD_SPI_ReadByte()
//#define SPIPutChar(x) SD_SPI_WriteByte(x)


/**********************************************************/
/*  �������� :  SPIPutChar                                */
/*  �������� �� ͨ��SPI����һ���ֽڵ�����                 */
/*  ����     :  �����͵��ֽ�����                          */
/*  ����ֵ   :  ��                                        */
/*--------------------------------------------------------*/


/*******************************************************************************************************************
** ��������: INT8U SPI_RecByte()				Name:	  INT8U SPI_RecByte()
** ��������: ��SPI�ӿڽ���һ���ֽ�				Function: receive a byte from SPI interface
** �䡡  ��: ��									Input:	  NULL
** �� �� ��: �յ����ֽ�							Output:	  the byte that be received
********************************************************************************************************************/
static uchar SPI_RecByte(void)
{
unsigned char clear;
SPDR=0xFF;
while(!(SPSR&(1<<SPIF)));
clear=SPSR;  
return SPDR ;
}




/*************************************************************/
/*  �������� :  Mp3WriteRegister                             */
/*  �������� �� дvs1003�Ĵ���                               */
/*  ����     :  �Ĵ�����ַ����д���ݵĸ�8λ����д���ݵĵ�8λ */
/*  ����ֵ   :  ��                                           */
/*-----------------------------------------------------------*/
void Mp3WriteRegister(unsigned char addressbyte, unsigned char highbyte, unsigned char lowbyte)
{
	Mp3DeselectData();
	Mp3SelectControl();//XCS = 0
	SPIPutChar(VS_WRITE_COMMAND); //����д�Ĵ�������
	SPIPutChar(addressbyte);      //���ͼĴ����ĵ�ַ
	SPIPutChar(highbyte);         //���ʹ�д���ݵĸ�8λ
	SPIPutChar(lowbyte);          //���ʹ�д���ݵĵ�8λ
	Mp3DeselectControl();
}

/*************************************************************/
/*  �������� :  Mp3ReadRegister                              */
/*  �������� �� дvs1003�Ĵ���                               */
/*  ����     :  �Ĵ�����ַ				     */
/*  ����ֵ   :  vs1003��16λ�Ĵ�����ֵ                       */
/*-----------------------------------------------------------*/
unsigned int Mp3ReadRegister(unsigned char addressbyte)
{
	 unsigned int resultvalue = 0;
	 uchar ucReadValue;

	Mp3DeselectData();//XDCS = 1
	Mp3SelectControl();//XCS = 0
	SPIPutChar(VS_READ_COMMAND); //���Ͷ��Ĵ�������
	SPIPutChar((addressbyte));	 //���ͼĴ����ĵ�ַ

//	SPIPutChar(0xff); 		//���Ͷ�ʱ��
//	resultvalue = (SPI_RESULT_BYTE) << 8;//��ȡ��8λ����
	ucReadValue = SPI_RecByte();
	resultvalue = ucReadValue<<8;
//	SPIPutChar(0xff);		   //���Ͷ�ʱ��
//	resultvalue |= (SPI_RESULT_BYTE);  //��ȡ��8λ����
	ucReadValue = SPI_RecByte();
	resultvalue |= ucReadValue;

	Mp3DeselectControl();              
	return resultvalue;                 //����16λ�Ĵ�����ֵ
}

/**********************************************************/
/*  �������� :  Mp3SoftReset                              */
/*  �������� �� vs1003�����λ                            */
/*  ����     :  ��                                        */
/*  ����ֵ   :  ��                                        */
/*--------------------------------------------------------*/
void Mp3SoftReset(void)
{
	Mp3WriteRegister (SPI_MODE, 0x08, 0x04); //�����λ

	wait(1); //��ʱ1ms
	while (MP3_DREQ == 0); //�ȴ������λ����
	Mp3WriteRegister(SPI_CLOCKF, 0x98, 0x00);//����vs1003��ʱ��,3��Ƶ
	Mp3WriteRegister(SPI_AUDATA, 0xBB, 0x81); //������48k��������
	Mp3WriteRegister(SPI_BASS, 0x00, 0x55);//��������
//	Mp3SetVolume(10,10);//��������                                           ---------------change-------------------
    wait(1); //��ʱ1ms
    	
    	//��vs1003����4���ֽ���Ч���ݣ���������SPI����
   	Mp3SelectData();
	SPIPutChar(0);
	SPIPutChar(0);
	SPIPutChar(0);
	SPIPutChar(0);
	Mp3DeselectData();

}
/**********************************************************/
/*  �������� :  Mp3Reset                                  */
/*  �������� �� vs1003Ӳ����λ                            */
/*  ����     :  ��                                        */
/*  ����ֵ   :  ��                                        */
/*--------------------------------------------------------*/
void Mp3Reset(void)
{	
	Mp3PutInReset();//xReset = 0   ��λvs1003      
	_delay_ms(100);//wait(200);//��ʱ100ms
	SPIPutChar(0xff);//����һ���ֽڵ���Ч���ݣ�����SPI����
	Mp3DeselectControl();   //xCS = 1
	Mp3DeselectData();     //xDCS = 1
	Mp3ReleaseFromReset(); //xRESET = 1
	_delay_ms(100);//wait(200);            //��ʱ100ms
	while (MP3_DREQ == 0);//�ȴ�DREQΪ��

    _delay_ms(100);//wait(200);            //��ʱ100ms
 //	Mp3SetVolume(50,50);                                            ---------------change-------------------
    Mp3SoftReset();//vs1003��λ
}


uchar CheckVS1003B_DRQ(void)
{
	uchar bResult;

	bResult =MP3_DREQ;
	return(bResult);
}

/***********************************************************/
/*  �������� :  VsSineTest                                 */
/*  �������� �� vs1003���Ҳ��ԣ����ú�������whileѭ���У�  */
/*              ����ܳ�������һ��һ�͵�������֤������ͨ�� */                            
/*  ����     :  ��                                         */
/*  ����ֵ   :  ��                                         */
/*---------------------------------------------------------*/
void VsSineTest(void)
{
	volatile unsigned int b;
	Mp3PutInReset();  //xReset = 0   ��λvs1003
	wait(200);        //��ʱ100ms        
	SPIPutChar(0xff);//����һ���ֽڵ���Ч���ݣ�����SPI����
	Mp3DeselectControl();  
	Mp3DeselectData();     
	Mp3ReleaseFromReset(); 
	wait(200);	               
//	Mp3SetVolume(50,50);                                             ---------------change-------------------

 	Mp3WriteRegister(SPI_MODE,0x08,0x20);//����vs1003�Ĳ���ģʽ
	b=Mp3ReadRegister(SPI_MODE);
       	while (MP3_DREQ == 0);     //�ȴ�DREQΪ��
 	Mp3SelectData();       //xDCS = 1��ѡ��vs1003�����ݽӿ�
 	
 	//��vs1003�������Ҳ������0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//����n = 0x24, �趨vs1003�����������Ҳ���Ƶ��ֵ��������㷽����vs1003��datasheet
   	SPIPutChar(0x53);      
	SPIPutChar(0xef);      
	SPIPutChar(0x6e);      
	SPIPutChar(0x24);      
	SPIPutChar(0x00);      
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
//	wait(500);
	wait(250);
	wait(250);
	Mp3DeselectData();//����ִ�е������Ӧ���ܴӶ�������һ����һƵ�ʵ�����
  
        //�˳����Ҳ���
	Mp3SelectData();
	SPIPutChar(0x45);
	SPIPutChar(0x78);
	SPIPutChar(0x69);
	SPIPutChar(0x74);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
//	wait(500);
	wait(250);
	wait(250);

	Mp3DeselectData();

        //�ٴν������Ҳ��Բ�����nֵΪ0x44���������Ҳ���Ƶ������Ϊ�����ֵ
    Mp3SelectData();       
	SPIPutChar(0x53);      
	SPIPutChar(0xef);      
	SPIPutChar(0x6e);      
	SPIPutChar(0x44);      
	SPIPutChar(0x00);      
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
//	wait(500);
	wait(250);
	wait(250);

	Mp3DeselectData(); 

	//�˳����Ҳ���
	Mp3SelectData();
	SPIPutChar(0x45);
	SPIPutChar(0x78);
	SPIPutChar(0x69);
	SPIPutChar(0x74);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
//	wait(500);
	wait(250);
	wait(250);

	Mp3DeselectData();
 }


void TestVS1003B(void)
{
	Mp3Reset();
	VsSineTest();
//	Mp3SoftReset();
//	test_1003_PlayMP3File();                                           ---------------change-------------------
}

//д�Ĵ�������������ַ������
void VS1003B_WriteCMD(unsigned char addr, unsigned int dat)
{
/*
	VS1003B_XDCS_H();
	VS1003B_XCS_L();
	VS1003B_WriteByte(0x02);
	//delay_Nus(20);
	VS1003B_WriteByte(addr);
	VS1003B_WriteByte(dat>>8);
	VS1003B_WriteByte(dat);
	//delay_Nus(200);
	VS1003B_XCS_H();
*/
	Mp3WriteRegister(addr,dat>>8,dat);
}

//���Ĵ��������� ��ַ ��������
unsigned int VS1003B_ReadCMD(unsigned char addr)
{
/*
	unsigned int temp;
	unsigned char temp1;
	VS1003B_XDCS_H();
	VS1003B_XCS_L();
	VS1003B_WriteByte(0x03);
	//delay_Nus(20);
	VS1003B_WriteByte(addr);
	temp=  VS1003B_ReadByte();
	temp=temp<<8;
	temp1= VS1003B_ReadByte();
	temp=temp|temp1;;
	VS1003B_XCS_H();
	return temp;
*/
	return(Mp3ReadRegister(addr));
}

//д���ݣ���������


//����������
void VS1003B_SetVirtualSurroundOn(void)
{
	uchar ucRepeatCount;
	uint uiModeValue;

	ucRepeatCount =0;

	while(1)//дʱ�ӼĴ���
	{
		uiModeValue = VS1003B_ReadCMD(0x00);
		if(uiModeValue & 0x0001)
		{
			break;
		}
		else
		{
			uiModeValue |= 0x0001;
			VS1003B_WriteCMD(0,uiModeValue);
		}
		ucRepeatCount++;
		if(ucRepeatCount++ >10 )break;
	}

}

//�رջ�����
void VS1003B_SetVirtualSurroundOff(void)
{
	uchar ucRepeatCount;
	uint uiModeValue;

	ucRepeatCount =0;

	while(1)//дʱ�ӼĴ���
	{
		uiModeValue = VS1003B_ReadCMD(0x00);
		if(uiModeValue & 0x0001)
		{
			break;
		}
		else
		{
			uiModeValue |= 0x0001;
			VS1003B_WriteCMD(0,uiModeValue);
		}
		ucRepeatCount++;
		if(ucRepeatCount++ >10 )break;
	}

}

//��ǿ����
//��ڲ���	1.ǿ��0-15
//			2.Ƶ��0-15 (X10Hz)
void VS1003B_SetBassEnhance(uchar ucValue,uchar ucFrequencyID)
{
	uchar ucRepeatCount;
	uint uiWriteValue;
	uint uiReadValue;	

	ucRepeatCount =0;

	uiWriteValue = VS1003B_ReadCMD(0x02);

	uiWriteValue &= 0xFF00;
	uiWriteValue |= ucValue<<4;
	uiWriteValue &= (ucFrequencyID & 0x0F);

	while(1)//дʱ�ӼĴ���
	{

		VS1003B_WriteCMD(2,uiWriteValue);
		uiReadValue = VS1003B_ReadCMD(0x02);
		
		if(uiReadValue == uiWriteValue)
		{
			break;
		}
		ucRepeatCount++;
		if(ucRepeatCount++ >10 )break;
	}

}


 

//VS1003��ʼ����0�ɹ� 1ʧ��

//VS1003�����λ
void VS1003B_SoftReset()
{
	VS1003B_WriteCMD(0x00,0x0804);//д��λ
//	VS1003B_Delay(0xffff);//��ʱ������1.35ms
	wait(2);
}

void VS1003B_Fill2048Zero()
{
	unsigned char i,j;

	for(i=0;i<64;i++)
	{
		if(CheckVS1003B_DRQ())
		{
			Mp3SelectData();

			for(j=0;j<32;j++)
			{

				VS1003B_WriteDAT(0x00);
			}
			Mp3DeselectData();
		}
	}
}


/*void test_1003_PlayMP3File() 
{
   unsigned int data_pointer;unsigned char i;
	unsigned int uiCount;

	uiCount =b;
	data_pointer=0; 
	VS1003B_SoftReset();
    while(uiCount>0)
  	{ 
		
	   if(CheckVS1003B_DRQ())
      	{
    		for(i=0;i<32;i++)
           	{
     			VS1003B_WriteDAT(MusicData[data_pointer]);
     			data_pointer++;
            }
			uiCount -= 32;
         }
    }
	VS1003B_Fill2048Zero();
}*/

struct string{ 

        char name[11]; 
		char attri;
		char leave[8];
		uint high;
		char leave1[4];
		uint low;
		
        unsigned long length; 
	

     }; 
	 #define  OPEN_PTT _PB3=0;
	 #define  CLOSE_PTT _PB3=1;
///                         �ж����õ���Ϣ���Ļظ����1-16              //////////////////////
//////////////////////���ŵ�N�׸���
struct string  *stu; 
void VS1003B_WriteMusic()
{
    volatile unsigned int data_pointer,i,j,flag;
	volatile unsigned long sector_pointer,len;
	data_pointer=0; 
	sector_pointer=32752+(stu->low*8);//65544 65520
	MP3_XDCS=0;
	b=0;
	len=stu->length;
	while(len>512)//len>2000
  	{ 
	
//		SD_Read_Sector(b++,lux);
//		_PB6=1;_PC6=1;
//		Init_SPI();	
		SD_Read_Sector(sector_pointer,lux);
		b=sector_pointer;
		sector_pointer++;
		// дһ������
		
 	 	for(i=0;i<16;i++)
      	{
			while(flag==0)
			{
				if(MP3_DREQ)
      			{
					for(j=0;j<32;j++)
     					{
						VS1003B_WriteDAT(lux[data_pointer]);
     					data_pointer++;
						}
				flag++;
            	}
			

		     }
			flag=0;		
			 
         }
		 	len -= 512;
			data_pointer=0; 
			
    }
	
	VS1003B_Fill2048Zero();
}



///////////////////////      �ŵ�target��  ���غ��滹�м����ļ�������        ///////////////////////////
uchar  transfer(unsigned char *target,unsigned char *buffer,unsigned char line)
{
	volatile unsigned char j,temp=0;
	volatile unsigned int length,i;
	length=32*line;//2��32���ֽ�Ϊһ���ļ�
	for(i=0;i<length;i++)
	{
	*buffer++;
	}

	for(j=0;j<32;j++)
	{
	target[j]=buffer[j];
	}	
	
	
}
void SetVolume(uint uiVolumeCount)
{
	uiVolumeCount|=uiVolumeCount<<8;	
	uchar retry=0;
	while(VS1003B_ReadCMD(0x0b) != uiVolumeCount)//������
	{
		VS1003B_WriteCMD(0x0b,uiVolumeCount);
		if(retry++ >10 )break;
	}
}

	

uchar WriteVoice(volatile uchar Number,uchar Volume)
{
		
		
		volatile uchar b,c=0;volatile uchar k=0,itmp,flag=0;
		volatile uint temp=Storage;
		stu=( struct string *)save;
		// ����Number
		_delay_ms(200);//���ܴ���400
			itmp = Number; 
            tar[0] = ( itmp / 100 )+48;
            itmp %= 100;
            tar[1] = ( itmp / 10 )+48;
            itmp %= 10;
            tar[2] = itmp+48;
       if(tar[0]==48)// ��ȥ����ǰ���0
	   for(c=0;c<3;c++)
	   tar[c]=tar[c+1];
	   if(tar[0]==48)
	   for(c=0;c<3;c++)
	   tar[c]=tar[c+1];
	 //��ȡǰ128��mp3�ľ�����Ϣ���Ҳ��Һ�Numberһ����

	for(b=0;b<8;b++)//�ʼ��8����
	{	
		SD_Read_Sector(temp++,lux);
		if(1==flag)break;
		for(k=0;k<16;k++)//ÿһ���ص�16���ļ�
		{
		transfer(save,lux,k);
		if(tar[0]==stu->name[0]&&tar[1]==stu->name[1]&&tar[2]==stu->name[2])
		{flag=1;break;}
		}
	}	
	temp=sector1;
	for(b=0;b<8;b++)//�ʼ��8����
	{	
		SD_Read_Sector(temp++,lux);
		if(1==flag)break;
		for(k=0;k<16;k++)//ÿһ���ص�16���ļ�
		{
		transfer(save,lux,k);
		if(tar[0]==stu->name[0]&&tar[1]==stu->name[1]&&tar[2]==stu->name[2])
		{flag=1;break;}
		}
	}	

	
		InitPortVS1003();
		SetVolume(Volume);
		VS1003B_WriteMusic();
		_delay_ms(200);//ȷ���������

		if(1==flag)	return 0;
		else return 1;
}

void VS1003B_WriteDAT(unsigned char dat)
{
//	VS1003B_XDCS_L();
//	VS1003B_WriteByte(dat);
//	VS1003B_XDCS_H();
//	VS1003B_XCS_H();

   	Mp3SelectData();
	SPIPutChar(dat);
//	SPIPutChar(dat);
	Mp3DeselectData();
	Mp3DeselectControl();

}
void SPIPutChar(unsigned char ucSendData)
{      
unsigned char clear;
SPDR=ucSendData;
while(!(SPSR&(1<<SPIF)));
clear=SPSR;  
//return SPDR ;
}


#define WT_CLK _PB7
#define WT_DI _PB5
#define WT_DO _PB6
unsigned long int WTH_2L(unsigned long int dat)
{
	unsigned char i;
	unsigned long int return_dat;
	
	WT_CLK = 1;
	_delay_us(100);
	for (i = 0; i < 24; i++)
	{
		WT_CLK = 1;
		if (dat & 0x800000) WT_DI=1;
		else WT_DI=0;
		dat <<= 1;
		_delay_us(50);  //50us
		
		if (WT_DO) return_dat |= 0x01;
		else return_dat &= ~(0x01);
		return_dat <<= 1;
		_delay_us(50);  //50us
		WT_CLK = 0;
		_delay_us(100); //100us
	}
	_delay_us(50);      //50us
	if (WT_DO) return_dat |= 0x01;
	else return_dat &= ~(0x01);
	return_dat &= 0x7ffffe;     //����ǰ�����õ�����
	
	return return_dat;
}

unsigned char Play_voice(unsigned char addr)
{
	unsigned long int dat;
	dat = 0x1800c8 + (addr << 5);
	if (WTH_2L(dat) == dat) return  1;   //���ųɹ�
	return 0;        //����ʧ��
}

unsigned char WTH_Check_sate(void)
{
	unsigned char i;
	unsigned long int dat = 0x2200;
	unsigned long int return_dat;
	
	WT_CLK = 1;
	_delay_us(200);
	for (i = 0; i < 16; i++)
	{
		WT_CLK = 1;
		if (dat & 0x8000) WT_DI = 1;
		else WT_DI = 0;
		dat <<= 1;
		_delay_us(50);  //�ȴ� 50us ��Ŷ�ȡ DO ����
		if (WT_DO) return_dat |= 0x01;
		else return_dat &= ~(0x01);
		return_dat <<= 1;
		
		_delay_us(50);  //50us
		WT_CLK = 0;
		_delay_us(100); //100us
	}
	if (WT_DO) return_dat |= 0x01;
	
	return_dat &= 0xffff;
	  if (return_dat & 0x80) //ֻ�жϵ� 9 λ����
	  return 1;       //���ڲ���
	  return 0;            //δ����
}	  

unsigned int value_rx[2]={};
unsigned char rxbuffer[32] = {0};
unsigned char luj[6]={32,32,32,32,33};

void main(void)
{
 		Init_MCU();
		Init_SPI();
		L01_CE_LOW( );
		L01_Init();
		L01_SetTRMode( RX_MODE );
		L01_WriteHoppingPoint( 0 );
		value=SD_Init();
		Init_SPI_HIGH();
		DREQ=0;	//dreq
		stu=( struct string *)save;
		
	//	for (int a=0;a<20;a++)
	//	lux[a]=0;
	
		if(value==0)
		{
		/*SD_write_sector(0,lux);
		SD_write_sector(1,lux);
		SD_write_sector(2,lux);
		SD_write_sector(3,lux);
		SD_write_sector(4,lux);*/
		
		SD_Read_Sector(4,lux);
		SD_Read_Sector(0,lux);
		SD_Read_Sector(Storage,lux);
		SD_Read_Sector(32775,lux);
		
		transfer(save,lux,15);	 
		sector1=32752+(stu->low*8)+stu->length/4096*8+8;
		SD_Read_Sector(sector1,lux);
		Mp3Reset();
		OPEN_PTT
		
		WriteVoice(253,0);
		_delay_ms(1000);
		WriteVoice(254,0);
		_delay_ms(3000);
		CLOSE_PTT}
		while(1)
		{    
	
//---------------change-----------------
			rxdata();
			OPEN_PTT
			_delay_ms(500);//ȷ������
			if(value==0)
			{
			if(rxbuffer[2]!=0)WriteVoice(rxbuffer[2],20);//?��????? value_rx[1]
			if(rxbuffer[0]!=0)WriteVoice(rxbuffer[0],20);
			if(rxbuffer[1]!=0)WriteVoice(rxbuffer[1],20);
			}
			CLOSE_PTT
	}
}

//VsSineTest();
//Mp3Reset();





/*	if(rxbuffer[4]!=32)
	i++;
	 itmp = value;
            testbuffer[0] = ( itmp / 10000 ) + '0'-16;
            itmp %= 10000;
            testbuffer[1] = ( itmp / 1000 ) + '0'-16;
            itmp %= 1000;
            testbuffer[2] = ( itmp / 100 ) + '0'-16;
            itmp %= 100;
            testbuffer[3] = ( itmp / 10 ) + '0'-16;
            itmp %= 10;
            testbuffer[4] = itmp + '0'-16;
            testbuffer[5] = 0;
			_PD3=0;
		if(rxbuffer[4]!=32||rxbuffer[3]!=32||rxbuffer[2]!=32)*/
//		LCD_Dis_Str( 6, 50, (char*)rxbuffer );
//		LCD_Dis_Str( 4, 50, (char*)testbuffer );
//	value=WriteVoice(3,20);