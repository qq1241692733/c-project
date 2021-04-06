#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"  
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
//ALIENTEK 探索者STM32F407开发板 实验40
//汉字显示 实验 -库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司    
//作者：正点原子 @ALIENTEK 

 
int main(void)
{        
	u32 fontcnt;		  
	u8 i,j;
	u8 fontx[2];//gbk码
	u8 key,t;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	LED_Init();					//初始化LED  
 	LCD_Init();					//LCD初始化  
	LCD_Display_Dir(1);
 	KEY_Init();					//按键初始化  
	W25QXX_Init();				//初始化W25Q128
	usmart_dev.init(168);		//初始化USMART
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
  	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
	while(font_init()) 			//检查字库
	{
UPD:    
	}  
	
	POINT_COLOR=RED;       
	LCD_ShowString(28,70,840,16,24, "--------------------------------------------------------------.");
	LCD_ShowString(28,85,840,16,24, "|            |           |           |           |           |"); 
	           Show_Str(28,100,840,24,"     项目     可回收垃圾   有害垃圾    厨余垃圾    其他垃圾",24,1);  
 
	LCD_ShowString(28,130,840,16,24,"|------------|-----------|-----------|-----------|-----------|");
	LCD_ShowString(28,145,840,16,24,"|            |           |           |           |           |"); 
          	Show_Str(28,160,840,24,"     数量 ",24,1 ); 

	LCD_ShowString(28,190,840,16,24,"|------------|-----------|-----------|-----------|-----------|");
	LCD_ShowString(28,205,840,16,24,"|            |           |           |           |           |"); 
	          Show_Str(28,220,840,24,"     顺序 ",24,1 ); 

	LCD_ShowString(28,250,840,16,24,"|------------|-----------|-----------|-----------|-----------|");
	LCD_ShowString(28,265,840,16,24,"|            |           |           |           |           |"); 
            Show_Str(28,280,840,24,"   满溢状态 ",24,1 ); 

	LCD_ShowString(28,310,840,16,24,"|------------|-----------|-----------|-----------|-----------|"); 

 	POINT_COLOR=BLUE;  
	
	Show_Str(360,30,200,24,"垃圾分类",24,0);				    	 

}







