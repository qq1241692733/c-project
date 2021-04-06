#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "beep.h"
#include "pwm.h"
#include "stepmotor.h"	

int main(void)
{
	u16 a=0;
	delay_init(168);
	time_init(20000,84-1);
	BEEP_Init();
	gpio_init();
	GPIO_SetBits(GPIOF,GPIO_Pin_4);    // 停

	while(1)         // PF10 CW转向    PF9 CLK脉冲    PF4 EN使能 
  	{	
			while(1)
			{
				a=garbage_ret();
				if(a!=0)
					break;
			}
			switch(a)   
			{
				
				case 1: 
						TIM_SetCompare1(TIM14,20000-1700);			
						delay_ms(1500);
						TIM_SetCompare1(TIM14,20000-1295);  //关
						delay_ms(2000);				
						a=0;
						break;
				
				case 2:   //2号桶
				  	Turn_around1(800,600);
						TIM_SetCompare1(TIM14,20000-1700);
						delay_ms(1200);
						Turn_around2(800,600);
						TIM_SetCompare1(TIM14,20000-1295);  //关
						delay_ms(2000);	
						a=0;
					break;
				
				case 3:   //3号桶
				  	Turn_around1(1600,600);
						TIM_SetCompare1(TIM14,20000-1700);
						delay_ms(1500);
						Turn_around2(1600,600);				
						TIM_SetCompare1(TIM14,20000-1295);  //关
						delay_ms(2000);
						a=0;				
					break;
				
				case 4:   // 4号桶
				  	Turn_around2(800,600);
						TIM_SetCompare1(TIM14,20000-1700);
						delay_ms(1500);
						Turn_around1(800,600);	
						TIM_SetCompare1(TIM14,20000-1295);  //关
						delay_ms(2000);	
						a=0;			
					break;
				
				default:
					break;
		}
	}	
}
