/*
 * sound.c
 *
 * Created: 2014/6/6 16:32:23
 *  Author: oyx
 */ 
#include <avr/io.h>
#include "macr.h"

#include "sound.h"
#include "sdcard.h"
#include "vs1003.h"
#include "nRF24L01.h"
#include "nRF24L01_Reg.h"



unsigned char lux[512]={1,1,1,1,1,1,1,1,1,11,1,1,1,1,1,1,1,1,1,11,1,1,1,1,1,1,1,11,1,1};
unsigned char tar[32]={32,32,32,32,32,32,32};
unsigned char save[32]={0};
volatile unsigned int value;


struct string  *stu;
void VS1003B_WriteMusic()
{
	volatile unsigned int data_pointer,i,j,flag;
	volatile unsigned long sector_pointer,len;
	data_pointer=0;
	sector_pointer=32752+(stu->low*8);//65544 65520
	MP3_XDCS=0;
//	b=0
	len=stu->length;
	while(len>512)//len>2000
	{
		SD_Read_Sector(sector_pointer,lux);
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
	volatile unsigned char j;
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


uchar WriteVoice(volatile uchar Number,uchar Volume)
{
	_PB4=0;
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

	
	InitPortVS1003();
	SetVolume(Volume);
	VS1003B_WriteMusic();
	_delay_ms(400);//确定播放完毕
	_PB4=1;
	if(1==flag)	return 0;
	else return 1;
}



unsigned value_rx[2]={};
unsigned char rxbuffer[32] = {"00000"};


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
	SD_Read_Sector(0,lux);
	Mp3Reset();
//	_delay_ms(1000);
	while(1)
	{                                           //---------------change-------------------
//		value_rx[1]=rxdata();
//		if (value_rx[0]==value_rx[1])
//		break;
//		VsSineTest();
		value=WriteVoice(21,0);//越小越大声 value_rx[1]
//		value_rx[0]=value_rx[1];
	}		
}
