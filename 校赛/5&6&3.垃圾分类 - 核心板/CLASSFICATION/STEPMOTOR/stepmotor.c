#include "stepmotor.h"	
#include "delay.h"
#include "lcd.h"

void gpio_init()
{
	GPIO_InitTypeDef GPIO_Initstructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	GPIO_Initstructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;   //Í¨ÐÅ
	GPIO_Initstructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_Initstructure.GPIO_PuPd=GPIO_PuPd_DOWN;
	GPIO_Init(GPIOF, &GPIO_Initstructure);
	
	GPIO_Initstructure.GPIO_Pin=GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;  // PF10 CW×ªÏò12 ºÚ   PF2 CLKÂö³å 11 °×    PF4 ENÊ¹ÄÜ 13
	GPIO_Initstructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_Initstructure.GPIO_OType=GPIO_OType_PP;
	GPIO_Initstructure.GPIO_Speed=GPIO_High_Speed;
	GPIO_Init(GPIOD, &GPIO_Initstructure);
	

	

}

void Turn_around1(int angle,int period)  //  Õý×ª
{
		int x;
		GPIO_ResetBits(GPIOD,GPIO_Pin_13);    // EN Ê¹ÄÜ 
		GPIO_SetBits(GPIOD,GPIO_Pin_12);    //  Õý×ª
		for(x = 0; x < angle; x++)            //  
		{
				GPIO_SetBits(GPIOD,GPIO_Pin_11);   //   CLKÀ­¸ß
				delay_us(period);
				GPIO_ResetBits(GPIOD,GPIO_Pin_11); //   CLKÀ­µÍ
				delay_us(period);    
		}
		GPIO_SetBits(GPIOD,GPIO_Pin_13);    // Í£
}

void Turn_around2(int angle,int period)  //  ·´×ª
{
		int x;
		GPIO_ResetBits(GPIOD,GPIO_Pin_13);    // EN Ê¹ÄÜ 
		GPIO_ResetBits(GPIOD,GPIO_Pin_12);    //  ·´×ª
		for(x = 0; x < angle; x++)            //  200¸öÖÜÆÚÎ»Îª6MsµÄÂö³å  Ò»¸öÂö³å1.8¡ã  1.8¡ã*200=360¡ãÒ»È¦1.2s  ²»Ï¸·Ö
		{
				GPIO_SetBits(GPIOD,GPIO_Pin_11);   //   CLKÀ­¸ß
				delay_us(period);
				GPIO_ResetBits(GPIOD,GPIO_Pin_11); //   CLKÀ­µÍ
				delay_us(period);    
		}
		GPIO_SetBits(GPIOD,GPIO_Pin_13);    // Í£
}



int garbage_ret()
{
	if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_1)==1)  //¿É»ØÊÕÀ¬»
	{
		return 1;			
	}
	if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_2)==1)  //ÓÐº¦À¬»ø
	{
		return 2;
	}
	if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_3)==1)  //³øÓàÀ¬»ø
	{
		return 3;	
	}
	if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_4)==1)  //ÆäËûÀ¬»ø
	{
		return 4;
	}
	else
		return 0;
}
