#include "stepmotor.h"	
#include "delay.h"

void gpio_init()
{
	GPIO_InitTypeDef GPIO_Initstructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	GPIO_Initstructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_10|GPIO_Pin_4;  // PF10 CW转向    PF2 CLK脉冲    PF4 EN使能 
	GPIO_Initstructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_Initstructure.GPIO_OType=GPIO_OType_PP;
	GPIO_Initstructure.GPIO_Speed=GPIO_High_Speed;
	GPIO_Init(GPIOF, &GPIO_Initstructure);
	
	GPIO_Initstructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;   //通信
	GPIO_Initstructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_Initstructure.GPIO_PuPd=GPIO_PuPd_DOWN;
	GPIO_Init(GPIOF, &GPIO_Initstructure);

}

void Turn_around1(int angle,int period)  //  正转
{
		int x;
		GPIO_ResetBits(GPIOF,GPIO_Pin_4);    // EN 使能 
		GPIO_SetBits(GPIOF,GPIO_Pin_10);    //  正转
		for(x = 0; x < angle; x++)            //  
		{
				GPIO_SetBits(GPIOF,GPIO_Pin_2);   //   CLK拉高
				delay_us(period);
				GPIO_ResetBits(GPIOF,GPIO_Pin_2); //   CLK拉低
				delay_us(period);    
		}
		GPIO_SetBits(GPIOF,GPIO_Pin_4);    // 停
}

void Turn_around2(int angle,int period)  //  反转
{
		int x;
		GPIO_ResetBits(GPIOF,GPIO_Pin_4);    // EN 使能 
		GPIO_ResetBits(GPIOF,GPIO_Pin_10);    //  反转
		for(x = 0; x < angle; x++)            //  200个周期位为6Ms的脉冲  一个脉冲1.8°  1.8°*200=360°一圈1.2s  不细分
		{
				GPIO_SetBits(GPIOF,GPIO_Pin_2);   //   CLK拉高
				delay_us(period);
				GPIO_ResetBits(GPIOF,GPIO_Pin_2); //   CLK拉低
				delay_us(period);    
		}
		GPIO_SetBits(GPIOF,GPIO_Pin_4);    // 停
}



int garbage_ret()
{
	if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_15)==1)
		return 1;			
	if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_12)==1)
		return 2;	
	if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_13)==1)
		return 3;	
	if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_14)==1)
		return 4;
	else
		return 0;
}
