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

void litter(int x, int y, int a) 
	{
	if(a == 1) {
			Show_Str(x,y,120,24,"可回收垃圾",24,0);		
	}
	if(a == 2) {
			Show_Str(x,y,100,24,"有害垃圾",24,0);		
	}
	if(a == 3) {
			Show_Str(x,y,100,24,"厨余垃圾",24,0);		
	}
	if(a == 4) {
			Show_Str(x,y,100,24,"其他垃圾",24,0);		
	}
}
	
void litter1(u8 n, u8 count, u8 a, u8 s) {
	if(n == 1) {
		Show_Str(30,60+30*count,780,24,"第 1 袋   数量:   类别:           ",24,0);
		LCD_ShowxNum(140,60+30*count,1,8,24,1);
		litter(324,60+30*count,a);
	}
	if(n == 2) {
		Show_Str(30,60+30*count,780,24,"第 2 袋   数量:   类别:           ",24,0);	
		LCD_ShowxNum(140,60+30*count,1,8,24,1);
		litter(324,60+30*count,a);
	}
	if(n == 3) {
		Show_Str(30,60+30*count,780,24,"第 3 袋   数量:   类别:           ",24,0);	
		LCD_ShowxNum(140,60+30*count,1,8,24,1);
		litter(324,60+30*count,a);
	}
}

void Display(void)
{    
	u8 a=0;
	u8 A=0;
	u8 B=0;
	u8 C=0;	
	u8 count=0;
	
	u8 n=0;

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
 	POINT_COLOR=BLUE;  
	//Show_Str(360,30,200,24,"垃圾",24,0);	
	time_init(20000,84-1);
	
//	while(1) {
//		a=garbage_ret();
//		LCD_ShowxNum(170,160,a,8,24,0);    //显示
//	}

	while(1) {              // PF10 CW转向    PF9 CLK脉冲    PF4 EN使能 
  //***************************************************************		
		Show_Str(360,30,200,24,"请投放垃圾",24,0);	
//		Show_Str(10,90,24,24,"  ",24,0);
//		Show_Str(10,120,24,24,"  ",24,0);
		if(count == 5){
			delay_ms(6000);  
			break;
		}
		if(garbage_ret()==1) {          //A袋垃圾 
		Show_Str(360,30,200,24,"正在  识别",24,0);	
			//litter1(n, count, a, A);
			n++;		
			delay_ms(2000);
			while(1) {
				while(1)
				{
					a=garbage_ret();
					if(a!=0) 
						break;
				}
				count ++;
				A++;
				
				litter1(n, count, a, A);
				Throw(a,A);
				a = 0;				
				Show_Str(480, 60+30*count, 200, 24, "OK!", 24, 1);
				
				if(A >= 1) break;		
			}
		}
  //***************************************************************
		if(garbage_ret()==2) {         //B袋垃圾
		Show_Str(360,30,200,24,"正在  识别",24,0);	
			n++;
			delay_ms(1000);
			while(1) {
				while(1)
				{
					a=garbage_ret();
					if(a!=0)					
						break;
				}
				count ++;
				B++;
				
				litter1(n, count, a, B);
				Throw(a,B);	
				a = 0;
				Show_Str(480, 60+30*count, 200, 24, "OK!", 24, 1);
				
				if(B == 2) break;	
			}

		} 
  //***************************************************************		
		if(garbage_ret()==3) {        //C袋垃圾
		Show_Str(360,30,200,24,"正在  识别",24,0);	
			n++;
			delay_ms(1000);
			while(1){
				while(1)
				{
					a=garbage_ret();
					if(a!=0)					
						break;
				}
				count ++;
				C++;
				
				litter1(n, count, a, C);
				Throw(a,C);	
				a = 0;
				Show_Str(480, 60+30*count, 200, 24, "OK!", 24, 1);
				
				if(C == 2) break;	
			}
		} 	
	}		
}


void Throw(u8 a, u8 cur) {
			GPIO_SetBits(GPIOF,GPIO_Pin_4);    // 停	
			//cur = cur / 2;
			switch(a)   
			{
				
				case 1:   //1号桶    可回收垃圾  PF1
						if(cur == 1) 
						{
							TIM_SetCompare1(TIM11,20000-2400);
							delay_ms(1200);
							TIM_SetCompare1(TIM11,20000-3180);  //关
						}
					  if(cur == 2)
						{
							TIM_SetCompare1(TIM13,20000-1680);  
							delay_ms(1200);
							TIM_SetCompare1(TIM13,20000-1250);  //关
						}
						break;
				
				case 2:   //2号桶    有害垃圾   PF13
						if(cur == 1) 
						{
							Turn_around1(800,600);
							TIM_SetCompare1(TIM11,20000-2400);
							delay_ms(1200);
							Turn_around2(800,600);
							TIM_SetCompare1(TIM11,20000-3180);  //关
							delay_ms(1200);
						}
					  if(cur == 2)
						{			
							Turn_around1(800,600);
							TIM_SetCompare1(TIM13,20000-1680);  
							delay_ms(1200);
							Turn_around2(800,600);
							TIM_SetCompare1(TIM13,20000-1250);  //关
							delay_ms(1200);
						}
				
					break;
				 
				case 3:   //3号桶     厨余垃圾   PF14
						if(cur == 1) 
						{
							Turn_around1(1600,600);
							TIM_SetCompare1(TIM11,20000-2400);
							delay_ms(1200);
							Turn_around2(1600,600);
							TIM_SetCompare1(TIM11,20000-3180);  //关
							delay_ms(1200);
						}
					  if(cur == 2)
						{			
							Turn_around1(1600,600);
							TIM_SetCompare1(TIM13,20000-1680);  
							delay_ms(1200);
							Turn_around2(1600,600);
							TIM_SetCompare1(TIM13,20000-1250);  //关
							delay_ms(1200);
						}
	
					break;
				
				case 4:   // 4号桶     其他垃圾   PF15
						if(cur == 1) 
						{
							Turn_around2(800,600);
							TIM_SetCompare1(TIM11,20000-2400);
							delay_ms(1200);
							Turn_around1(800,600);	
							TIM_SetCompare1(TIM11,20000-3180);  //关
							delay_ms(1200);
						}
					  if(cur == 2)
						{			
							Turn_around2(800,600);
							TIM_SetCompare1(TIM13,20000-1680);  
							delay_ms(1200);
							Turn_around1(800,600);	
							TIM_SetCompare1(TIM13,20000-1250);  //关
							delay_ms(1200);
						}
				
					break;
				
				default:
					break;
				}
}
	


