#include "beep.h" 
#include "delay.h"

#include "sys.h"
#include "lcd.h"
#include "sram.h"   
#include "malloc.h" 
#include "usmart.h"  
#include "sdio_sdcard.h"    
#include "malloc.h" 
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	

//BEEP IO初始化
void BEEP_Init(void)
{   
  GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOF时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOF时钟
	
  //初始化蜂鸣器对应引脚GPIOF8
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
  GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化GPIO
  GPIO_ResetBits(GPIOF,GPIO_Pin_8);  //蜂鸣器对应引脚GPIOF8拉低，

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;   //满载
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIO
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);//PE2 连接到中断线2	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource1);//PE2 连接到中断线2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);//PE2 连接到中断线2	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);//PE2 连接到中断线2	
	
  EXTI_InitStructure.EXTI_Line = EXTI_Line0|EXTI_Line1|EXTI_Line2|EXTI_Line3;//LINE0
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //上升沿触发 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE0
  EXTI_Init(&EXTI_InitStructure);//配置	

	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//外部中断1
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;//外部中断1
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//外部中断1
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//外部中断1
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
}


void EXTI0_IRQHandler(void)
{	  
	if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0)==0)       
	{
		delay_ms(100);     
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0)==0)    
			{
        Show_Str(246,280,840,24,"满",24,1 ); 				
				GPIO_SetBits(GPIOF,GPIO_Pin_8);   //报警
				delay_ms(300);                 
				GPIO_ResetBits(GPIOF,GPIO_Pin_8); 
				delay_ms(10); 
    	
			}
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}

void EXTI1_IRQHandler(void)
{		  	    	
	if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1)==0)       
	{
		delay_ms(100);     
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1)==0)    
			{
        Show_Str(390,280,840,24,"满",24,1 ); 		
				GPIO_SetBits(GPIOF,GPIO_Pin_8);   //报警
				delay_ms(300);                 
				GPIO_ResetBits(GPIOF,GPIO_Pin_8); 
				delay_ms(10); 
				
			}
	}
	EXTI_ClearITPendingBit(EXTI_Line1);
}
void EXTI2_IRQHandler(void)
{	  	    	
	if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)==0)       
	{
		delay_ms(100);     
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)==0)    
			{
        Show_Str(534,280,840,24,"满",24,1 ); 				
				GPIO_SetBits(GPIOF,GPIO_Pin_8);   //报警
				delay_ms(300);                 
				GPIO_ResetBits(GPIOF,GPIO_Pin_8); 
				delay_ms(10); 
			}
	}
	EXTI_ClearITPendingBit(EXTI_Line2);
}

void EXTI3_IRQHandler(void)
{	  	    	
	if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)==0)       
	{
		delay_ms(100);     
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)==0)    
			{
        Show_Str(678,280,840,24,"满",24,1 ); 					
				GPIO_SetBits(GPIOF,GPIO_Pin_8);   //报警
				delay_ms(300);                 
				GPIO_ResetBits(GPIOF,GPIO_Pin_8); 
				delay_ms(10); 
			}
	}
	EXTI_ClearITPendingBit(EXTI_Line3);
}
