#include "display.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
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
#include "beep.h"
#include "pwm.h"
#include "stepmotor.h"	

void dis(u8 count, u8 a) {
		LCD_ShowxNum(60,130+ 24*count,count,8,24,0);    //显示
		Show_Str(60,130+ 24*count,840,24,"                           1         OK！",24,0);
		LCD_ShowxNum(60,130+ 24*count, count, 8, 24, 0);    //显示
		if(a == 1) {
			Show_Str(200,130+ 24*count,840,24,"可回收垃圾",24,1);  
		}else if(a == 2) {
			Show_Str(200,130+ 24*count,840,24,"有害垃圾垃圾",24,1);  
		}else if(a== 3) {
			Show_Str(200,130+ 24*count,840,24,"厨余垃圾",24,1);  
		}else if(a == 4) {
			Show_Str(200,130+ 24*count,840,24,"其他垃圾",24,1);  
		}
		
		if(count == 7) {
			Show_Str(200,346,840,24,"投放完成！",24,1);  
		}
}

void Display(void)
{    
	u8 a=0;
	extern u8 count;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200

	gpio_init();	

	LED_Init();					//初始化LED  
 	LCD_Init();					//LCD 初始化 	
	LCD_Display_Dir(1);

	W25QXX_Init();				//初始化W25Q128
	usmart_dev.init(168);		//初始化USMART
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
  	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH
	
	while(font_init()) 			//检查字库
	{
UPD:    
	}  
	POINT_COLOR=RED;   
	LCD_ShowString(28,70,840,16,24, "---------------------------------------------------");
	Show_Str(28,100,840,24,"     序号    垃圾类别     数量      成功与否",24,1);  
	POINT_COLOR=BLUE;
	Show_Str(360,30,200,24,"垃圾分类",24,0);	

	time_init(20000,84-1);
	while(1) {
		while(1)   // PF10 CW转向    PF9 CLK脉冲    PF4 EN使能 
		{
			a=garbage_ret();
			if(a==2)					
				break;
		}
		delay_ms(200);
		a = 0;
		while(1) {
			a=garbage_ret();
			if(a!=0)					
				break;
		}
		
		GPIO_ResetBits(GPIOD,GPIO_Pin_13);    // EN 使能 
		delay_ms(20);
		switch(a)   
		{
			GPIO_ResetBits(GPIOD,GPIO_Pin_13);    // EN 使能 
					TIM_SetCompare1(TIM10,20000-3110);  //关	
					TIM_SetCompare1(TIM13,20000-1090);  //关
			
			case 1:   //1号桶    可回收垃圾  PF1			 
					count++;
					dis( count, a);
			
					TIM_SetCompare1(TIM10,20000-2160);  //关	 左13
					TIM_SetCompare1(TIM13,20000-1660);  //关
					delay_ms(1200);		
					TIM_SetCompare1(TIM10,20000-3090);  //关	
					TIM_SetCompare1(TIM13,20000-1140);  //关
			
					a=0;	
					break;
			
			case 2:   //2号桶    有害垃圾   PF13		
					count++;		
					dis( count, a);
						
					Turn_around1(800,600);
					TIM_SetCompare1(TIM10,20000-2160);  //关	 左13
					TIM_SetCompare1(TIM13,20000-1660);  //关
					delay_ms(1200);		
					TIM_SetCompare1(TIM10,20000-3090);  //关	
					TIM_SetCompare1(TIM13,20000-1140);  //关
					Turn_around2(800,600);

					a=0;				
				break;
			 
			case 3:   //3号桶     厨余垃圾   PF14
					count++;
					dis( count, a);
									
					Turn_around1(1600,600);
					TIM_SetCompare1(TIM10,20000-2160);  //关	 左13
					TIM_SetCompare1(TIM13,20000-1660);  //关
					delay_ms(1200);		
		
					TIM_SetCompare1(TIM10,20000-3090);  //关	
					TIM_SetCompare1(TIM13,20000-1140);  //关
					Turn_around2(1600,600);				
		
					a=0;
				break;
			
			case 4:   // 4号桶     其他垃圾   PF15
					count++;	
					dis( count, a);
					
					Turn_around2(800,600);
					TIM_SetCompare1(TIM10,20000-2160);  //关	 左13
					TIM_SetCompare1(TIM13,20000-1660);  //关
					delay_ms(1200);
					
					TIM_SetCompare1(TIM10,20000-3090);  //关	
					TIM_SetCompare1(TIM13,20000-1140);  //关
					Turn_around1(800,600);	
					a=0;		
				break;
			
			default:
				break;
			}
		GPIO_ResetBits(GPIOD,GPIO_Pin_13);    // EN 使能 
	}
	
}
