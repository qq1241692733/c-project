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


void Display(void)
{    
	u8 a1=0;
	u8 a2=0;
	u8 a3=0;
	u8 a4=0;	
  u8 a=0;
	u8 count=0;	

	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200


	BEEP_Init();
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
	LCD_ShowString(28,70,840,16,24, "--------------------------------------------------------------.");
	LCD_ShowString(28,85,840,16,24, "|            |           |           |           |           |"); 
	           Show_Str(28,100,840,24,"     项目     可回收垃圾   有害垃圾    厨余垃圾    其他垃圾",24,1);  
 
	LCD_ShowString(28,130,840,16,24,"|------------|-----------|-----------|-----------|-----------|");
	LCD_ShowString(28,145,840,16,24,"|            |           |           |           |           |"); 
          	Show_Str(28,160,840,24,"     数量 ",24,1 ); 

	LCD_ShowString(28,190,840,16,24,"|------------|-----------|-----------|-----------|-----------|");
	LCD_ShowString(28,205,840,16,24,"|            |           |           |           |           |"); 
//	LCD_ShowString(28,220,840,16,24,"                  --          --          --          --      ");	
	          Show_Str(28,220,840,24,"     顺序 ",24,1 ); 

	LCD_ShowString(28,250,840,16,24,"|------------|-----------|-----------|-----------|-----------|");
	LCD_ShowString(28,265,840,16,24,"|            |           |           |           |           |"); 
//	LCD_ShowString(28,280,840,16,24,"                  --          --          --          --      ");			
            Show_Str(28,280,840,24,"   满溢状态       --          --          --          --      ",24,1 ); 

	LCD_ShowString(28,310,840,16,24,"|------------|-----------|-----------|-----------|-----------|"); 


 	POINT_COLOR=BLUE;  
	Show_Str(360,30,200,24,"垃圾分类",24,0);	
	LCD_ShowxNum(170,160,a1,8,24,0);    //显示
	LCD_ShowxNum(314,160,a2,8,24,0);    //显示
  LCD_ShowxNum(458,160,a3,8,24,0);    //显示	
	LCD_ShowxNum(602,160,a4,8,24,0);    //显示		
	

	
						
	time_init(20000,84-1);

	while(1)         // PF10 CW转向    PF9 CLK脉冲    PF4 EN使能 
  	{	
		
			while(1)
			{
//				if(EXTI_GetITStatus(EXTI_Line1|EXTI_Line2|EXTI_Line3|EXTI_Line0))
//				{

//				}

				a=garbage_ret();
				if(a!=0)					
					break;
			}

			GPIO_SetBits(GPIOF,GPIO_Pin_4);    // 停	
			switch(a)   
			{
				
				case 1:   //1号桶    可回收垃圾  PF1
						a1++;		
					  LCD_ShowxNum(170,160,a1,8,24,0);    //显示
				
						count++;				
						LCD_ShowxNum((a-1)*144+120+(a1-1)*35,220,count,8,24,1);    //显示	
						TIM_SetCompare1(TIM13,20000-1550);  //关				
						TIM_SetCompare1(TIM11,20000-1450);		
						delay_ms(1200);
						TIM_SetCompare1(TIM13,20000-1950);
						TIM_SetCompare1(TIM11,20000-700);  //关
						delay_ms(2000);	
						
						a=0;				
						break;
				
				case 2:   //2号桶    有害垃圾   PF13
						a2++;					
						LCD_ShowxNum(314,160,a2,8,24,0);    //显示	
				
						count++;					
						LCD_ShowxNum((a-1)*144+120+(a2-1)*35,220,count,8,24,1);    //显示		
	
				  	Turn_around1(800,600);
						TIM_SetCompare1(TIM13,20000-1550);  //关				
						TIM_SetCompare1(TIM11,20000-1450);	
						delay_ms(1200);
						Turn_around2(800,600);
						TIM_SetCompare1(TIM13,20000-1950);
						TIM_SetCompare1(TIM11,20000-700);  //关
						delay_ms(2000);	
				

						a=0;				
					break;
				 
				case 3:   //3号桶     厨余垃圾   PF14
						a3++;
						LCD_ShowxNum(458,160,a3,8,24,0);    //显示	
				
						count++;						
						LCD_ShowxNum((a-1)*144+120+(a3-1)*35,220,count,8,24,1);    //显示		

				
				  	Turn_around1(1600,600);
						TIM_SetCompare1(TIM13,20000-1550);  //关				
						TIM_SetCompare1(TIM11,20000-1450);	
						delay_ms(1500);
						Turn_around2(1600,600);				
						TIM_SetCompare1(TIM13,20000-1950);
						TIM_SetCompare1(TIM11,20000-700);  //关
						delay_ms(2000);
						a=0;
					break;
				
				case 4:   // 4号桶     其他垃圾   PF15
						a4++;		
						LCD_ShowxNum(602,160,a4,8,24,0);    //显示	
				
						count++;				
						LCD_ShowxNum((a-1)*144+120+(a4-1)*35,220,count,8,24,1);    //显示		

				
				  	Turn_around2(800,600);
						TIM_SetCompare1(TIM13,20000-1550);  //关				
						TIM_SetCompare1(TIM11,20000-1450);	
						delay_ms(1500);
						Turn_around1(800,600);	
						TIM_SetCompare1(TIM13,20000-1950);
						TIM_SetCompare1(TIM11,20000-700);  //关
						delay_ms(2000);	
						a=0;		
					break;
				
				default:
					break;
		}
	}	
}
