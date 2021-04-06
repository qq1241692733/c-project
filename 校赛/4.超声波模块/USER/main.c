#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "time.h"
#include "beep.h" 
//ALIENTEK 探索者STM32F407开发板 实验13
//LCD显示实验-库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK

int main(void)
{ 
	int tim = 0;
	int s = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);      //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	
	LED_Init();					  //初始化LED
 	LCD_Init();           //初始化LCD FSMC接口
	POINT_COLOR=RED;      //画笔颜色：红色	
	TIM3_Int_Init(2000,84-1);
	LCD_ShowString(30,136,210,24,24,"S(mm):");	
  while(1) 
	{	
		PBout(8)=1;
		delay_us(20);
		PBout(8)=0;
		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == 0);
		TIM_Cmd(TIM3,ENABLE);
		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == 1);
		TIM_Cmd(TIM3,DISABLE);
		tim = TIM_GetCounter(TIM3);
		TIM3->CNT = 0;
		s = tim * 0.34 / 2;
		LCD_ShowxNum(30,160, s,6,24,0);		
	} 
}
