#ifndef __I2S_H
#define __I2S_H
#include "sys.h"    									
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//I2S 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/24
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//V1.1 20140606
//新增I2S2ext_Init、I2S2ext_RX_DMA_Init、I2S_Rec_Start和I2S_Rec_Stop等函数
////////////////////////////////////////////////////////////////////////////////// 	
 

extern void (*i2s_tx_callback)(void);		//IIS TX回调函数指针 
extern void (*i2s_rx_callback)(void);		//IIS RX回调函数指针 

void I2S2_Init(u16 I2S_Standard,u16 I2S_Mode,u16 I2S_Clock_Polarity,u16 I2S_DataFormat);
void I2S2ext_Init(u16 I2S_Standard,u16 I2S_Mode,u16 I2S_Clock_Polarity,u16 I2S_DataFormat);
u8 I2S2_SampleRate_Set(u32 samplerate);
void I2S2_TX_DMA_Init(u8* buf0,u8 *buf1,u16 num);
void I2S2ext_RX_DMA_Init(u8* buf0,u8 *buf1,u16 num);
void I2S_Play_Start(void);
void I2S_Rec_Start(void);
void I2S_Play_Stop(void);
void I2S_Rec_Stop(void);
 

#endif





















