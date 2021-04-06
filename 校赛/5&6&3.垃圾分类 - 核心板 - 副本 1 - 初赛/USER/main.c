#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "beep.h" 
#include "lcd.h"
#include "key.h"  
#include "display.h"
#include "picture.h"
#include "video.h"
#include "usmart.h"  
#include "malloc.h" 
#include "w25qxx.h"    
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	 
#include "wm8978.h"	 
#include "videoplayer.h" 
#include "timer.h" 

#include "display.h"
  u8 count=0;
int main(void)
{ 
  u8 key;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	KEY_Init();   
	uart_init(115200);		//初始化串口波特率为115200

	LED_Init();					//初始化LED  
 	LCD_Init();					//LCD 初始化 	
	LCD_Display_Dir(1);  //横屏

	W25QXX_Init();				//初始化W25Q128
	usmart_dev.init(168);		//初始化USMART
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
  	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH	
	while(font_init()) 			//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
		delay_ms(200);	
		LED0=!LED0;
	}  	 
	POINT_COLOR=RED;    
  
	BEEP_Init();
	while(1)
	{
			video_Display();
			Display();	
//		key=KEY_Scan(0);		//得到键值
//	   	if(key)
//		{						   
//			switch(key)
//			{				 
//				case WKUP_PRES:	//控制蜂鸣器
//					Display();
//					break;
//				case KEY0_PRES:	//控制LED0翻转

//					break;
//			}
//		}else delay_ms(10); 
	}
}


