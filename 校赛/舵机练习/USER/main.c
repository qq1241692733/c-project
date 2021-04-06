#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "pwm.h"
#include "key.h"

int main(void)
{ 
  u8 key;           //保存键值
	u16 led0pwmval=0;  
	delay_init(168);  //初始化延时函数
	LED_Init();				//初始化LED端口 
	uart_init(115200);//初始化串口波特率为115200
	KEY_Init();       //初始化与按键连接的硬件接口
	LED0=0;				  	//先点亮红灯
	TIM14_PWM_Init(20000,84-1);
	while(1)
	{
	//	TIM_SetCompare1(TIM14,led0pwmval);
		key=KEY_Scan(0);		//得到键值
	   	if(key)
		{					
			switch(key)
			{				 
				case WKUP_PRES:	//
				TIM_SetCompare1(TIM14,led0pwmval);
				led0pwmval=20000-1300;    //0.5ms  45°   30°  0.333ms
				delay_ms(1000);				
				TIM_SetCompare1(TIM14,led0pwmval);
				led0pwmval=20000-1700;
				delay_ms(1000);
					break;
				case KEY0_PRES:	//
					led0pwmval=20000-800;
					LED0=!LED0;
					break;
				case KEY1_PRES:	// 
					LED1=!LED1;
				TIM_SetCompare1(TIM14,led0pwmval);
				led0pwmval=20000-1700;
					break;
	//			case KEY2_PRES:	//旋转180°
		//		led0pwmval=20000-2500;
	//				break;
			}		
		}
			else delay_ms(20); 
	}

}
