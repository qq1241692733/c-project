#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
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
//ALIENTEK 探索者STM32F407开发板 实验45
//视频播放器实验 -库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司    
//作者：正点原子 @ALIENTEK 

 
int main(void)
{ 
  u8 key;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	
	LED_Init();					//初始化LED 

	usmart_dev.init(84);		//初始化USMART
 	LCD_Init();					//LCD初始化  
	LCD_Display_Dir(1);
 	KEY_Init();					//按键初始化   
	W25QXX_Init();				//初始化W25Q128
	WM8978_Init();				//初始化WM8978	
	
	WM8978_ADDA_Cfg(1,0);		//开启DAC
	WM8978_Input_Cfg(0,0,0);	//关闭输入通道
	WM8978_Output_Cfg(1,0);		//开启DAC输出  
	WM8978_HPvol_Set(40,40);
	WM8978_SPKvol_Set(60);
	TIM3_Int_Init(10000-1,8400-1);//10Khz计数,1秒钟中断一次
	
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
  	f_mount(fs[0],"0:",1); 		//挂载SD卡  
	POINT_COLOR=RED;      
	while(font_init()) 			//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
		delay_ms(200);	
		LED0=!LED0;
	}  	 
	POINT_COLOR=RED;      
	Show_Str(360,30,200,24,"垃圾分类",24,0);	
	delay_ms(1500);

		while(1)
	{
		key=KEY_Scan(0);		//得到键值
	   	if(key)
		{						   
			switch(key)
			{				 
				case WKUP_PRES:	//控制蜂鸣器
					video_play();
					break;
				case KEY0_PRES:	//控制LED0翻转
					LED0=!LED0;
					break;
				case KEY1_PRES:	//控制LED1翻转	 
					LED1=!LED1;
					break;
				case KEY2_PRES:	//同时控制LED0,LED1翻转 
					LED0=!LED0;
					LED1=!LED1;
					break;
			}
		}else delay_ms(10); 
	}
}


