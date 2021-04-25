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
#include "piclib.h"	
#include "string.h"	
#include "math.h"	
#include "picture.h"

u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//临时目录
	FILINFO tfileinfo;	//临时文件信息	
	u8 *fn;	 			 			   			     
    res=f_opendir(&tdir,(const TCHAR*)path); 	//打开目录
  	tfileinfo.lfsize=_MAX_LFN*2+1;				//长文件名最大长度
	tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);//为长文件缓存区分配内存
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)//查询总的有效文件数
		{
	        res=f_readdir(&tdir,&tfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||tfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出		  
     		fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				rval++;//有效文件数增加1
			}	    
		}  
	} 
	return rval;
}


void picture_Dispaly(){
	u8 res;
 	DIR picdir;	 		//图片目录
	FILINFO picfileinfo;//文件信息
	u8 *fn;   			//长文件名
	u8 *pname;			//带路径的文件名
	u16 totpicnum; 		//图片文件总数
	u16 curindex;		//图片当前索引
	u8 key;				//键值
	u8 pause=0;			//暂停标记
	u8 t;
	u16 temp;
	u16 *picindextbl;	//图片索引表 　
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	LED_Init();					//初始化LED 
	usmart_dev.init(84);		//初始化USMART
 	LCD_Init();					//LCD初始化  
 	KEY_Init();					//按键初始化  
	LCD_Display_Dir(1);
	W25QXX_Init();				//初始化W25Q128
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();			//为fatfs相关变量申请内存  
  	f_mount(fs[0],"0:",1); 	//挂载SD卡 
 	f_mount(fs[1],"1:",1); 	//挂载FLASH.
	POINT_COLOR=RED;      
	while(font_init()) 		//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	 
 	Show_Str(30,50,200,16,"Explorer STM32F4开发板",16,0);				    	 
	Show_Str(30,70,200,16,"图片显示程序",16,0);				    	 
	Show_Str(30,90,200,16,"KEY0:NEXT KEY1:PREV",16,0);				    	 
	Show_Str(30,110,200,16,"WK_UP:PAUSE",16,0);				    	 
	Show_Str(30,130,200,16,"正点原子@ALIENTEK",16,0);				    	 
	Show_Str(30,150,200,16,"2014年5月15日",16,0);
 	while(f_opendir(&picdir,"0:/PICTURE"))//打开图片文件夹
 	{	    
		Show_Str(30,170,240,16,"PICTURE文件夹错误!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	}  
	totpicnum=pic_get_tnum("0:/PICTURE"); //得到总有效文件数
  	while(totpicnum==NULL)//图片文件为0		
 	{	    
		Show_Str(30,170,240,16,"没有图片文件!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	}
  	picfileinfo.lfsize=_MAX_LFN*2+1;						//长文件名最大长度
	picfileinfo.lfname=mymalloc(SRAMIN,picfileinfo.lfsize);	//为长文件缓存区分配内存
 	pname=mymalloc(SRAMIN,picfileinfo.lfsize);				//为带路径的文件名分配内存
 	picindextbl=mymalloc(SRAMIN,2*totpicnum);				//申请2*totpicnum个字节的内存,用于存放图片索引
 	while(picfileinfo.lfname==NULL||pname==NULL||picindextbl==NULL)//内存分配出错
 	{	    
		Show_Str(30,170,240,16,"内存分配失败!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	
	//记录索引
    res=f_opendir(&picdir,"0:/PICTURE"); //打开目录
	if(res==FR_OK)
	{
		curindex=0;//当前索引为0
		while(1)//全部查询一遍
		{
			temp=picdir.index;								//记录当前index
	        res=f_readdir(&picdir,&picfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出		  
     		fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				picindextbl[curindex]=temp;//记录索引
				curindex++;
			}	    
		} 
	}   
	Show_Str(30,170,240,16,"开始显示...",16,0); 
	delay_ms(1500);
	piclib_init();										//初始化画图	   	   
	curindex=0;											//从0开始显示
   	res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//打开目录
	while(res==FR_OK)//打开成功
	{	
		dir_sdi(&picdir,picindextbl[curindex]);			//改变当前目录索引	   
        res=f_readdir(&picdir,&picfileinfo);       		//读取目录下的一个文件
        if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出
     	fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
		strcpy((char*)pname,"0:/PICTURE/");				//复制路径(目录)
		strcat((char*)pname,(const char*)fn);  			//将文件名接在后面
 		LCD_Clear(BLACK);
 		ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//显示图片    
		Show_Str(2,2,240,16,pname,16,1); 				//显示图片名字
		t=0;
		while(1) 
		{
			key=KEY_Scan(0);		//扫描按键
			if(t>250)key=1;			//模拟一次按下KEY0    
			if((t%20)==0)LED0=!LED0;//LED0闪烁,提示程序正在运行.
			if(key==KEY1_PRES)		//上一张
			{
				if(curindex)curindex--;
				else curindex=totpicnum-1;
				break;
			}else if(key==KEY0_PRES)//下一张
			{
				curindex++;		   	
				if(curindex>=totpicnum)curindex=0;//到末尾的时候,自动从头开始
				break;
			}else if(key==WKUP_PRES)
			{
				pause=!pause;
				LED1=!pause; 	//暂停的时候LED1亮.  
			}
			if(pause==0)t++;
			delay_ms(10); 
		}					    
		res=0;  
	} 											  
	myfree(SRAMIN,picfileinfo.lfname);	//释放内存			    
	myfree(SRAMIN,pname);				//释放内存			    
	myfree(SRAMIN,picindextbl);			//释放内存		
}
