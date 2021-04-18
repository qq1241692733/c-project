#include "pwm.h"
#include "stm32f4xx.h"


void time_init(u32 arr,u32 psc)
{
	GPIO_InitTypeDef GPIO_Initstructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13,ENABLE);  	//TIM14时钟使能    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11,ENABLE);  	//TIM14时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10,ENABLE);  	//
	
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource8,GPIO_AF_TIM13); //GPIOF9复用为定时器14
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource7,GPIO_AF_TIM11); //GPIOF9复用为定时器14
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource6,GPIO_AF_TIM10); //GPIOF9复用为定时器14

	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6;           //GPIOF9
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOF,&GPIO_Initstructure);              //初始化PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM13,&TIM_TimeBaseStructure);//初始化定时器14
	TIM_TimeBaseInit(TIM11,&TIM_TimeBaseStructure);//初始化定时器14
	
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM10,&TIM_TimeBaseStructure);//初始化定时器14
	//初始化TIM14 Channel1 PWM模式	 
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低
	TIM_OC1Init(TIM13, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1
	TIM_OC1Init(TIM11, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1f
	TIM_OC1Init(TIM10, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1
	
	TIM_OC1PreloadConfig(TIM13, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器
 	TIM_OC1PreloadConfig(TIM11, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器
 	TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器
	
  TIM_ARRPreloadConfig(TIM13,ENABLE);//ARPE使能 
  TIM_ARRPreloadConfig(TIM11,ENABLE);//ARPE使能 	
  TIM_ARRPreloadConfig(TIM10,ENABLE);//ARPE使能 	
	
	TIM_Cmd(TIM13, ENABLE);  //使能TIM14
	//TIM_Cmd(TIM11, ENABLE);  //使能TIM14
	TIM_Cmd(TIM10, ENABLE);  //使能TIM14			
}


