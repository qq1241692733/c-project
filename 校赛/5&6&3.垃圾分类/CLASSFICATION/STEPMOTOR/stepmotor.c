#include "stepmotor.h"	
#include "delay.h"
#include "lcd.h"

void gpio_init()
{
	GPIO_InitTypeDef GPIO_Initstructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	GPIO_Initstructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_10|GPIO_Pin_4;  // PF10 CW×ªÏò    PF2 CLKÂö³å    PF4 ENÊ¹ÄÜ 
	GPIO_Initstructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_Initstructure.GPIO_OType=GPIO_OType_PP;
	GPIO_Initstructure.GPIO_Speed=GPIO_High_Speed;
	GPIO_Init(GPIOF, &GPIO_Initstructure);
	
	GPIO_Initstructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;   //Í¨ÐÅ
	GPIO_Initstructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_Initstructure.GPIO_PuPd=GPIO_PuPd_DOWN;
	GPIO_Init(GPIOF, &GPIO_Initstructure);

}

void Turn_around1(int angle,int period)  //  Õý×ª
{
		int x;
		GPIO_ResetBits(GPIOF,GPIO_Pin_4);    // EN Ê¹ÄÜ 
		GPIO_SetBits(GPIOF,GPIO_Pin_10);    //  Õý×ª
		for(x = 0; x < angle; x++)            //  
		{
				GPIO_SetBits(GPIOF,GPIO_Pin_2);   //   CLKÀ­¸ß
				delay_us(period);
				GPIO_ResetBits(GPIOF,GPIO_Pin_2); //   CLKÀ­µÍ
				delay_us(period);    
		}
		GPIO_SetBits(GPIOF,GPIO_Pin_4);    // Í£
}

void Turn_around2(int angle,int period)  //  ·´×ª
{
		int x;
		GPIO_ResetBits(GPIOF,GPIO_Pin_4);    // EN Ê¹ÄÜ 
		GPIO_ResetBits(GPIOF,GPIO_Pin_10);    //  ·´×ª
		for(x = 0; x < angle; x++)            //  200¸öÖÜÆÚÎ»Îª6MsµÄÂö³å  Ò»¸öÂö³å1.8¡ã  1.8¡ã*200=360¡ãÒ»È¦1.2s  ²»Ï¸·Ö
		{
				GPIO_SetBits(GPIOF,GPIO_Pin_2);   //   CLKÀ­¸ß
				delay_us(period);
				GPIO_ResetBits(GPIOF,GPIO_Pin_2); //   CLKÀ­µÍ
				delay_us(period);    
		}
		GPIO_SetBits(GPIOF,GPIO_Pin_4);    // Í£
}


static u8 b1=0;
//u8 b2=0;
//u8 b3=0;
//u8 b4=0;	
int garbage_ret()
{
	if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_1)==1)  //¿É»ØÊÕÀ¬»
	{
		b1=1;
		return 1;			
	}
	if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_13)==1)  //ÓÐº¦À¬»ø
	{
//		extern u8 b2=1;
		return 2;
	}
	if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_14)==1)  //³øÓàÀ¬»ø
	{
//		b3++;
		return 3;	
	}
	
	if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_15)==1)  //ÆäËûÀ¬»ø
	{
//		b4++;
		return 4;
	}
	else
		return 0;
}
