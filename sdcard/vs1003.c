/*
 * MP3模块测试程序
 * 
 * 用途：MP3模块测试程序
 *       vs1003 的硬件测试程序，主控芯片为STC12LE5A60S2
 *       其他的微处理器(带SPI接口的)只需稍加修改即可适用
 *       对于不带硬SPI接口的微处理器可以用IO进行SPI的时序模拟
 * 
 * 作者					日期				备注
 * Huafeng Lin			20010/09/10			新增
 * Huafeng Lin			20010/09/10			修改
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

//针对SD卡读写板调整引脚
#define uint8 unsigned char

void LCD_write_english_string(unsigned char X,unsigned char Y,char *s);

//#define SPIWait()	{ while((S0SPSR & 0x80) == 0); }//等待SPI将数据发送完毕

//#define SPI_RESULT_BYTE  S0SPDR

//extern long volatile timeval; //用于延时的全局变量
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
/*  函数名称 :   MSPI_Init                                */
/*  函数功能 ： 初始化SPI接口，设置为主机。               */
/*  参数     :  无                                        */
/*  返回值   :  无                                        */
/*--------------------------------------------------------*/
void  MSPI_Init(void)
{  
/*
	PINSEL0 = (PINSEL0 & 0xFFFF00FF) | 0x00005500;	//选择 SPI 
        S0SPCCR = 0x08;		                        // SPI 时钟设置
 	S0SPCR  = (0 << 3) |				// CPHA = 0, 
 		  (0 << 4) |				// CPOL = 0, 
 		  (1 << 5) |				// MSTR = 1, 
 		  (0 << 6) |				// LSBF = 0, 
 	          (0 << 7);				// SPIE = 0, 
*/
//	DDRA=0xff;
//	DDR_PA2=0;/////////dreq为输入                              ---------------change----------------
//	DDR_PA5=0;/////////so为输入
	DDR_PA7=0;
//	c_SPI_SO = 1;
//	MP3_DREQ = 1;

}

/**********************************************************/
/*  函数名称 :  InitPortVS1003                            */
/*  函数功能 ： MCU与vs1003接口的初始化                   */
/*  参数     :  无                                        */
/*  返回值   :  无                                        */
/*--------------------------------------------------------*/
void  InitPortVS1003(void)
{
	MSPI_Init();//SPI口的初始化
//	IODIR &= 0xfffeffff;   //其他接口线的设置，其中dreq 为输入口
//	IODIR |= MP3_XRESET | MP3_XCS | MP3_XDCS;//xRESET，xCS，xDS均为输出口
//	IOSET |= MP3_XRESET | MP3_XCS | MP3_XDCS;//xRESET，xCS，xDS默认输出高电平	
//	MP3_DREQ = 1;		//置为输入

//	MP3_XRESET = 1;
	MP3_XCS = 1;
	MP3_XDCS = 0;
}

//uint8 SD_SPI_ReadByte(void);
//void SD_SPI_WriteByte(uint8 ucSendData);

//#define SPI_RecByte()  SD_SPI_ReadByte()
//#define SPIPutChar(x) SD_SPI_WriteByte(x)


/**********************************************************/
/*  函数名称 :  SPIPutChar                                */
/*  函数功能 ： 通过SPI发送一个字节的数据                 */
/*  参数     :  待发送的字节数据                          */
/*  返回值   :  无                                        */
/*--------------------------------------------------------*/


/*******************************************************************************************************************
** 函数名称: INT8U SPI_RecByte()				Name:	  INT8U SPI_RecByte()
** 功能描述: 从SPI接口接收一个字节				Function: receive a byte from SPI interface
** 输　  入: 无									Input:	  NULL
** 输 　 出: 收到的字节							Output:	  the byte that be received
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
/*  函数名称 :  Mp3WriteRegister                             */
/*  函数功能 ： 写vs1003寄存器                               */
/*  参数     :  寄存器地址，待写数据的高8位，待写数据的低8位 */
/*  返回值   :  无                                           */
/*-----------------------------------------------------------*/
void Mp3WriteRegister(unsigned char addressbyte, unsigned char highbyte, unsigned char lowbyte)
{
	Mp3DeselectData();
	Mp3SelectControl();//XCS = 0
	SPIPutChar(VS_WRITE_COMMAND); //发送写寄存器命令
	SPIPutChar(addressbyte);      //发送寄存器的地址
	SPIPutChar(highbyte);         //发送待写数据的高8位
	SPIPutChar(lowbyte);          //发送待写数据的低8位
	Mp3DeselectControl();
}

/*************************************************************/
/*  函数名称 :  Mp3ReadRegister                              */
/*  函数功能 ： 写vs1003寄存器                               */
/*  参数     :  寄存器地址				     */
/*  返回值   :  vs1003的16位寄存器的值                       */
/*-----------------------------------------------------------*/
unsigned int Mp3ReadRegister(unsigned char addressbyte)
{
	 unsigned int resultvalue = 0;
	 uchar ucReadValue;

	Mp3DeselectData();//XDCS = 1
	Mp3SelectControl();//XCS = 0
	SPIPutChar(VS_READ_COMMAND); //发送读寄存器命令
	SPIPutChar((addressbyte));	 //发送寄存器的地址

//	SPIPutChar(0xff); 		//发送读时钟
//	resultvalue = (SPI_RESULT_BYTE) << 8;//读取高8位数据
	ucReadValue = SPI_RecByte();
	resultvalue = ucReadValue<<8;
//	SPIPutChar(0xff);		   //发送读时钟
//	resultvalue |= (SPI_RESULT_BYTE);  //读取低8位数据
	ucReadValue = SPI_RecByte();
	resultvalue |= ucReadValue;

	Mp3DeselectControl();              
	return resultvalue;                 //返回16位寄存器的值
}

/**********************************************************/
/*  函数名称 :  Mp3SoftReset                              */
/*  函数功能 ： vs1003软件复位                            */
/*  参数     :  无                                        */
/*  返回值   :  无                                        */
/*--------------------------------------------------------*/
void Mp3SoftReset(void)
{
	Mp3WriteRegister (SPI_MODE, 0x08, 0x04); //软件复位

	wait(1); //延时1ms
	while (MP3_DREQ == 0); //等待软件复位结束
	Mp3WriteRegister(SPI_CLOCKF, 0x98, 0x00);//设置vs1003的时钟,3倍频
	Mp3WriteRegister(SPI_AUDATA, 0xBB, 0x81); //采样率48k，立体声
	Mp3WriteRegister(SPI_BASS, 0x00, 0x55);//设置重音
//	Mp3SetVolume(10,10);//设置音量                                           ---------------change-------------------
    wait(1); //延时1ms
    	
    	//向vs1003发送4个字节无效数据，用以启动SPI发送
   	Mp3SelectData();
	SPIPutChar(0);
	SPIPutChar(0);
	SPIPutChar(0);
	SPIPutChar(0);
	Mp3DeselectData();

}
/**********************************************************/
/*  函数名称 :  Mp3Reset                                  */
/*  函数功能 ： vs1003硬件复位                            */
/*  参数     :  无                                        */
/*  返回值   :  无                                        */
/*--------------------------------------------------------*/
void Mp3Reset(void)
{	
	Mp3PutInReset();//xReset = 0   复位vs1003      
	_delay_ms(100);//wait(200);//延时100ms
	SPIPutChar(0xff);//发送一个字节的无效数据，启动SPI传输
	Mp3DeselectControl();   //xCS = 1
	Mp3DeselectData();     //xDCS = 1
	Mp3ReleaseFromReset(); //xRESET = 1
	_delay_ms(100);//wait(200);            //延时100ms
	while (MP3_DREQ == 0);//等待DREQ为高

    _delay_ms(100);//wait(200);            //延时100ms
 //	Mp3SetVolume(50,50);                                            ---------------change-------------------
    Mp3SoftReset();//vs1003软复位
}


uchar CheckVS1003B_DRQ(void)
{
	uchar bResult;

	bResult =MP3_DREQ;
	return(bResult);
}

/***********************************************************/
/*  函数名称 :  VsSineTest                                 */
/*  函数功能 ： vs1003正弦测试，将该函数放在while循环中，  */
/*              如果能持续听到一高一低的声音，证明测试通过 */                            
/*  参数     :  无                                         */
/*  返回值   :  无                                         */
/*---------------------------------------------------------*/
void VsSineTest(void)
{
	volatile unsigned int b;
	Mp3PutInReset();  //xReset = 0   复位vs1003
	wait(200);        //延时100ms        
	SPIPutChar(0xff);//发送一个字节的无效数据，启动SPI传输
	Mp3DeselectControl();  
	Mp3DeselectData();     
	Mp3ReleaseFromReset(); 
	wait(200);	               
//	Mp3SetVolume(50,50);                                             ---------------change-------------------

 	Mp3WriteRegister(SPI_MODE,0x08,0x20);//进入vs1003的测试模式
	b=Mp3ReadRegister(SPI_MODE);
       	while (MP3_DREQ == 0);     //等待DREQ为高
 	Mp3SelectData();       //xDCS = 1，选择vs1003的数据接口
 	
 	//向vs1003发送正弦测试命令：0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//其中n = 0x24, 设定vs1003所产生的正弦波的频率值，具体计算方法见vs1003的datasheet
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
	Mp3DeselectData();//程序执行到这里后应该能从耳机听到一个单一频率的声音
  
        //退出正弦测试
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

        //再次进入正弦测试并设置n值为0x44，即将正弦波的频率设置为另外的值
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

	//退出正弦测试
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

//写寄存器，参数，地址和数据
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

//读寄存器，参数 地址 返回内容
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

//写数据，音乐数据


//开启环绕声
void VS1003B_SetVirtualSurroundOn(void)
{
	uchar ucRepeatCount;
	uint uiModeValue;

	ucRepeatCount =0;

	while(1)//写时钟寄存器
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

//关闭环绕声
void VS1003B_SetVirtualSurroundOff(void)
{
	uchar ucRepeatCount;
	uint uiModeValue;

	ucRepeatCount =0;

	while(1)//写时钟寄存器
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

//增强重音
//入口参数	1.强度0-15
//			2.频率0-15 (X10Hz)
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

	while(1)//写时钟寄存器
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


 

//VS1003初始化，0成功 1失败

//VS1003软件复位
void VS1003B_SoftReset()
{
	VS1003B_WriteCMD(0x00,0x0804);//写复位
//	VS1003B_Delay(0xffff);//延时，至少1.35ms
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
///                         判断有用的信息在哪回复序号1-16              //////////////////////
//////////////////////播放第N首歌曲
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
		// 写一个扇区
		
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



///////////////////////      放到target中  返回后面还有几个文件的数据        ///////////////////////////
uchar  transfer(unsigned char *target,unsigned char *buffer,unsigned char line)
{
	volatile unsigned char j,temp=0;
	volatile unsigned int length,i;
	length=32*line;//2排32个字节为一个文件
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
	while(VS1003B_ReadCMD(0x0b) != uiVolumeCount)//设音量
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
		// 分析Number
		_delay_ms(200);//不能大于400
			itmp = Number; 
            tar[0] = ( itmp / 100 )+48;
            itmp %= 100;
            tar[1] = ( itmp / 10 )+48;
            itmp %= 10;
            tar[2] = itmp+48;
       if(tar[0]==48)// 除去名字前面的0
	   for(c=0;c<3;c++)
	   tar[c]=tar[c+1];
	   if(tar[0]==48)
	   for(c=0;c<3;c++)
	   tar[c]=tar[c+1];
	 //读取前128个mp3的具体信息并且查找和Number一样的

	for(b=0;b<8;b++)//最开始的8个簇
	{	
		SD_Read_Sector(temp++,lux);
		if(1==flag)break;
		for(k=0;k<16;k++)//每一个簇的16个文件
		{
		transfer(save,lux,k);
		if(tar[0]==stu->name[0]&&tar[1]==stu->name[1]&&tar[2]==stu->name[2])
		{flag=1;break;}
		}
	}	
	temp=sector1;
	for(b=0;b<8;b++)//最开始的8个簇
	{	
		SD_Read_Sector(temp++,lux);
		if(1==flag)break;
		for(k=0;k<16;k++)//每一个簇的16个文件
		{
		transfer(save,lux,k);
		if(tar[0]==stu->name[0]&&tar[1]==stu->name[1]&&tar[2]==stu->name[2])
		{flag=1;break;}
		}
	}	

	
		InitPortVS1003();
		SetVolume(Volume);
		VS1003B_WriteMusic();
		_delay_ms(200);//确定播放完毕

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
	return_dat &= 0x7ffffe;     //屏蔽前后无用的数据
	
	return return_dat;
}

unsigned char Play_voice(unsigned char addr)
{
	unsigned long int dat;
	dat = 0x1800c8 + (addr << 5);
	if (WTH_2L(dat) == dat) return  1;   //播放成功
	return 0;        //播放失败
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
		_delay_us(50);  //等待 50us 后才读取 DO 数据
		if (WT_DO) return_dat |= 0x01;
		else return_dat &= ~(0x01);
		return_dat <<= 1;
		
		_delay_us(50);  //50us
		WT_CLK = 0;
		_delay_us(100); //100us
	}
	if (WT_DO) return_dat |= 0x01;
	
	return_dat &= 0xffff;
	  if (return_dat & 0x80) //只判断第 9 位数据
	  return 1;       //正在播放
	  return 0;            //未播放
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
			_delay_ms(500);//确保开启
			if(value==0)
			{
			if(rxbuffer[2]!=0)WriteVoice(rxbuffer[2],20);//?С????? value_rx[1]
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