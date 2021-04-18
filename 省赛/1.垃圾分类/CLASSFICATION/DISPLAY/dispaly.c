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


void ReDisplay(u8 a1, u8 a2, u8 a3, u8 a4, u8 arr1[], u8 arr2[], u8 arr3[], u8 arr4[])
{
	u8 i1;
	u8 i2;
	u8 i3;
	u8 i4;
	
	LCD_ShowxNum(170,160,a1,8,24,0);
	LCD_ShowxNum(314,160,a2,8,24,0);	
	LCD_ShowxNum(458,160,a3,8,24,0);    
  LCD_ShowxNum(602,160,a4,8,24,0);    

//********************************************************************	
	for(i1 = 0; i1 < a1; i1 ++)  //可回收 a1
	{	
		if(i1 < 4)
		{
				LCD_ShowxNum(120+ i1*35,220,arr1[i1],8,24,1);    //显示	
		}else
		{
				LCD_ShowxNum(120+(i1-4)*35,244,arr1[i1],8,24,1);    //显示	
		}				
	}
//********************************************************************	
	for(i2 = 0; i2 < a2; i2++) //有害 a2
	{	
		if(i2 < 4)
		{
				LCD_ShowxNum(144+120+ i2*35,220,arr2[i2],8,24,1);    //显示	
		}else
		{
				LCD_ShowxNum(144+120+(i2-4)*35,244,arr2[i2],8,24,1);    //显示	
		}				
	}
//********************************************************************	
	for(i3 = 0; i3 < a3; i3++) //厨余 a2
	{	
		if(i3 < 4)
		{
				LCD_ShowxNum(144*2+120+ i3*35,220,arr3[i3],8,24,1);    //显示	
		}else
		{
				LCD_ShowxNum(144*2+120+(i3-4)*35,244,arr3[i3],8,24,1);    //显示	
		}				
	}	
//********************************************************************	
	for(i4 = 0; i4 < a4; i4++) //
	{	
		if(i4 < 4)
		{
				LCD_ShowxNum(144*3+120+ i4*35,220,arr4[i4],8,24,1);    //显示	
		}else
		{
				LCD_ShowxNum(144*3+120+(i4-4)*35,244,arr4[i4],8,24,1);    //显示	
		}				
	}	
}


void Display(void)
{    
	u8 a=0;
	extern u8 count;
	
	extern u8 a1;
	extern u8 a2;
	extern u8 a3;
	extern u8 a4;	
	extern u8 arr1[10];
	extern u8 arr2[10];
	extern u8 arr3[10];
	extern u8 arr4[10];
	
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
	LCD_ShowString(28,70,840,16,24, "--------------------------------------------------------------");
	LCD_ShowString(28,85,840,16,24, "|            |           |           |           |           |"); 
	           Show_Str(28,100,840,24,"     项目     可回收垃圾   有害垃圾    厨余垃圾    其他垃圾",24,1);  
 
	LCD_ShowString(28,130,840,16,24,"|------------|-----------|-----------|-----------|-----------|");
	LCD_ShowString(28,145,840,16,24,"|            |           |           |           |           |"); 
          	Show_Str(28,160,840,24,"     数量 ",24,1 ); 

	LCD_ShowString(28,190,840,16,24,"|------------|-----------|-----------|-----------|-----------|");
	LCD_ShowString(28,205,840,16,24,"|            |           |           |           |           |");
	
	LCD_ShowString(28,220,840,16,24,"|            |           |           |           |           |"); 
	LCD_ShowString(28,235,840,16,24,"|            |           |           |           |           |"); 
	LCD_ShowString(28,240,840,16,24,"|            |           |           |           |           |"); 	
	          Show_Str(28,232,840,24,"     序号 ",24,1 ); 

	LCD_ShowString(28,280,840,16,24,"|------------|-----------|-----------|-----------|-----------|");
	LCD_ShowString(28,295,840,16,24,"|            |           |           |           |           |"); 
////	LCD_ShowString(28,280,840,16,24,"                  --          --          --          --      ");			
            Show_Str(28,310,840,24,"   满溢状态       --          --          --          --      ",24,1 ); 

	LCD_ShowString(28,340,840,16,24,"|------------|-----------|-----------|-----------|-----------|"); 

 	POINT_COLOR=BLUE;
	Show_Str(360,30,200,24,"垃圾分类",24,0);	
	LCD_ShowxNum(170,160,a1,8,24,0);    //显示
	LCD_ShowxNum(314,160,a2,8,24,0);    //显示
  LCD_ShowxNum(458,160,a3,8,24,0);    //显示	
	LCD_ShowxNum(602,160,a4,8,24,0);    //显示		
	ReDisplay(a1, a2, a3, a4, arr1, arr2, arr3, arr4);
	
	time_init(20000,84-1);
	
	while(1)   // PF10 CW转向    PF9 CLK脉冲    PF4 EN使能 
	{
		isfull();
		a=garbage_ret();
		if(a!=0)					
			break;
	}
	
	GPIO_SetBits(GPIOF,GPIO_Pin_4);    // 停	
	switch(a)   
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_13);    // 停
		case 1:   //1号桶    可回收垃圾  PF1
				a1++;		
				LCD_ShowxNum(170,160,a1,8,24,0);    //显示垃某类垃圾总数				 
				count++;
				arr1[a1-1] = count;
		
				if(a1 <= 4)
				{
						LCD_ShowxNum((a-1)*144+120+(a1-1)*35,220,count,8,24,1);    //显示	
				}else
				{
						LCD_ShowxNum((a-1)*144+120+(a1-5)*35,244,count,8,24,1);    //显示	
				}							
			
		
				TIM_SetCompare1(TIM10,20000-2150);  //关	
				TIM_SetCompare1(TIM13,20000-1650);  //关
				GPIO_SetBits(GPIOD,GPIO_Pin_13);    // 停
				delay_ms(1200);		
				TIM_SetCompare1(TIM10,20000-3110);  //关	
				TIM_SetCompare1(TIM13,20000-1110);  //关
				delay_ms(1200);
				
				Show_Str(336,408,200,24,"  可回收垃圾  OK",24,0);
				POINT_COLOR=RED;  
				LCD_ShowxNum(260,408,count,8,24,1);    //显示序号
				LCD_ShowxNum(406,408,a1,8,24,1);    //显示						
				POINT_COLOR=BLUE;			
				
				a=0;	
				break;
		
		case 2:   //2号桶    有害垃圾   PF13
				a2++;					
				LCD_ShowxNum(314,160,a2,8,24,0);    //显示	
				count++;		
				arr2[a2 - 1] = count;
		
				if(a2 <= 4)
				{
						LCD_ShowxNum((a-1)*144+120+(a2-1)*35,220,count,8,24,1);    //显示	
				}else
				{
						LCD_ShowxNum((a-1)*144+120+(a2-5)*35,244,count,8,24,1);    //显示	
				}	
					
				Turn_around1(800,600);
				GPIO_SetBits(GPIOD,GPIO_Pin_13);    // 停
				TIM_SetCompare1(TIM10,20000-2150);  //关	
				TIM_SetCompare1(TIM13,20000-1650);  //关
				GPIO_SetBits(GPIOD,GPIO_Pin_13);    // 停
				delay_ms(2500);		
				Turn_around2(800,600);
				TIM_SetCompare1(TIM10,20000-3110);  //关	
				TIM_SetCompare1(TIM13,20000-1110);  //关
				delay_ms(1200);
				
				Show_Str(336,408,200,24,"  有害垃圾    OK",24,0);
				POINT_COLOR=RED;  
				LCD_ShowxNum(260,408,count,8,24,1);    //显示
				LCD_ShowxNum(406,408,a2,8,24,1);    //显示						
				POINT_COLOR=BLUE;						

				a=0;				
			break;
		 
		case 3:   //3号桶     厨余垃圾   PF14
				a3++;
				LCD_ShowxNum(458,160,a3,8,24,0);    //显示	 
				count++;
				arr3[a3 - 1] = count;
		
				if(a3 <= 4)
				{
						LCD_ShowxNum((a-1)*144+120+(a3-1)*35,220,count,8,24,1);    //显示	
				}else
				{
						LCD_ShowxNum((a-1)*144+120+(a3-5)*35,244,count,8,24,1);    //显示	
				}		
									
				Turn_around1(1600,600);
				TIM_SetCompare1(TIM10,20000-2150);  //关	
				TIM_SetCompare1(TIM13,20000-1650);  //关
						GPIO_SetBits(GPIOD,GPIO_Pin_13);    // 停
				delay_ms(2000);		
				Turn_around2(1600,600);				
				TIM_SetCompare1(TIM10,20000-3110);  //关	
				TIM_SetCompare1(TIM13,20000-1110);  //关
				delay_ms(1200);
		
				Show_Str(336,408,200,24,"  厨余垃圾    OK",24,0);
				POINT_COLOR=RED;  
				LCD_ShowxNum(260,408,count,8,24,1);    //显示
				LCD_ShowxNum(406,408,a3,8,24,1);    //显示						
				POINT_COLOR=BLUE;				
				a=0;
			break;
		
		case 4:   // 4号桶     其他垃圾   PF15
				a4++;		
				LCD_ShowxNum(602,160,a4,8,24,0);    //显示	
				count++;	
				arr4[a4 - 1] = count;
		
				LCD_ShowxNum((a-1)*144+120+(a4-1)*35,220,count,8,24,1);    //显示		
		
				Turn_around2(800,600);
				TIM_SetCompare1(TIM10,20000-2150);  //关	
				TIM_SetCompare1(TIM13,20000-1650);  //关;
				GPIO_SetBits(GPIOD,GPIO_Pin_13);    // 停
				delay_ms(1200);		
				Turn_around1(800,600);	
				TIM_SetCompare1(TIM10,20000-3110);  //关	
				TIM_SetCompare1(TIM13,20000-1110);  //关
				delay_ms(1200);	

				Show_Str(336,408,200,24,"  其他垃圾    OK",24,0);
				POINT_COLOR=RED;  
				LCD_ShowxNum(260,408,count,8,24,1);    //显示
				LCD_ShowxNum(406,408,a4,8,24,1);    //显示						
				POINT_COLOR=BLUE;					
				a=0;		
			break;
		
		default:
			break;
		}
	GPIO_SetBits(GPIOD,GPIO_Pin_13);    // 停
	delay_ms(6000);
}
	


