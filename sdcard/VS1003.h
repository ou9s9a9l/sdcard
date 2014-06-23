#ifndef VS10XX_H
#define VS10XX_H

/* Include processor definition */
//#include <msp430x14x.h>

//vs1003��غ궨�� 
#define VS_WRITE_COMMAND 0x02    //д����
#define VS_READ_COMMAND  0x03    //������
#define SPI_MODE         0x00    //����ģʽ   
#define SPI_STATUS       0x01    //VS1003 ״̬   
#define SPI_BASS         0x02    //���ø�/������ǿ��   
#define SPI_CLOCKF       0x03    //ʱ��Ƶ�� + ��Ƶ��   
#define SPI_DECODE_TIME  0x04    //ÿ��������  
#define SPI_AUDATA       0x05    //��Ƶ����   
#define SPI_WRAM         0x06    //RAM ��д  
#define SPI_WRAMADDR     0x07    //RAM ��д��ַ  
#define SPI_HDAT0        0x08    //��ͷ����0  
#define SPI_HDAT1        0x09    //��ͷ����1  
#define SPI_AIADDR       0x0A    //Ӧ�ó�����ʼ��ַ  
#define SPI_VOL          0x0B    //��������  
#define SPI_AICTRL0      0x0C    //Ӧ�ÿ��ƼĴ���0 
#define SPI_AICTRL1      0x0D    //Ӧ�ÿ��ƼĴ���1   
#define SPI_AICTRL2      0x0E    //Ӧ�ÿ��ƼĴ���2  
#define SPI_AICTRL3      0x0F    //Ӧ�ÿ��ƼĴ���3  
/*#define SM_DIFF         0x01   
#define SM_JUMP         0x02   
#define SM_RESET        0x04   
#define SM_OUTOFWAV     0x08   
#define SM_PDOWN        0x10   
#define SM_TESTS        0x20   
#define SM_STREAM       0x40   
#define SM_PLUSV        0x80   
#define SM_DACT         0x100   
#define SM_SDIORD       0x200   
#define SM_SDISHARE     0x400   
#define SM_SDINEW       0x800   
#define SM_ADPCM        0x1000   
#define SM_ADPCM_HP     0x2000 */

//msp430-149��vs1003�Ľӿڶ���
/*
  I/O         NAME
  P5.4      /xCS
  P5.5        /xDCS
  P5.3        SCLK
  P5.2        SO
  P5.1        SI        
  P5.6        DREQ    
  P5.0       /xRESET 
*/
//#define MP3_XRESET   ( 1 << 0)
 
/*#define Mp3PutInReset()  { P5OUT &= ~MP3_XRESET; }  //xReset = 0
 
#define Mp3ReleaseFromReset()  { P5OUT |= MP3_XRESET; }//xReset = 1
 
//#define MP3_XCS  ( 1 << 4)
#define Mp3SelectControl()  {P5OUT &= ~MP3_XCS; }//xCS = 0
 
#define Mp3DeselectControl()  {P5OUT |= MP3_XCS; }//xCS = 1
 
//#define MP3_XDCS ( 1 << 5 )

#define Mp3SelectData()		{ P5OUT &= ~MP3_XDCS; }//xDCS = 0

#define Mp3DeselectData()	{ P5OUT |= MP3_XDCS; }//xDCS = 1

//#define MP3_DREQ ( 1 << 6 )

//#define SPIWait()	while((IFG2&UTXIFG1)==0);//�ȴ�...ֱ�������жϱ�־λ��λ                 //{ while((S0SPSR & 0x80) == 0); }//�ȴ�SPI�����ݷ������

//#define SPI_RESULT_BYTE  S0SPDR                                              

//#define Mp3SetVolume(leftchannel,rightchannel)  {Mp3WriteRegister(11,(leftchannel),(rightchannel));}//�������� 
		
		 */

void VS_Reset(); //VS1003��λ����ʼ��
void VS_Write_Reg(unsigned char addr,unsigned char hdat,unsigned char ldat); //��VS1003�Ĺ��ܼĴ���д��һ����
unsigned int VS_Read_Reg(unsigned char addr); //��VS1003�Ĺ��ܼĴ�����ȡһ����
void VS_Send_Dat(unsigned char dat); //��VS1003������Ƶ����
void VS_Flush_Buffer(); //���VS1003�����ݻ�����
void VS_sin_test(unsigned char x); //���Ҳ���
void LoadPatch(); //ΪVS1003�򲹶�

#endif

#define vs1003_c      0
#if vs1003_c==1
#define  MP3_XRESET   _PA6

 
#define MP3_XCS  _PC7
 

 
#define MP3_XDCS   _PC6


#define MP3_DREQ  READ_PA7
#define DREQ      DDR_PA7

#elif  vs1003_c==0// �Լ���Ƭ��

#define  MP3_XRESET   _PB2
 

 
#define MP3_XCS  _PD3
 

 
#define MP3_XDCS   _PD4



#define MP3_DREQ  READ_PD5
#define DREQ      DDR_PD5
#endif
