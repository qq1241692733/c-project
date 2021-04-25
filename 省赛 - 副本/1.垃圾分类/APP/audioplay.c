#include "audioplay.h"
#include "ff.h"
#include "malloc.h"
#include "usart.h"
#include "wm8978.h"
#include "i2s.h"
#include "led.h"
#include "lcd.h"
#include "delay.h"
#include "key.h"
#include "exfuns.h"  
#include "text.h"
#include "string.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//音乐播放器 应用代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/24
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
 

//音乐播放控制器
__audiodev audiodev;	  
 

//开始音频播放
void audio_start(void)
{
	audiodev.status=3<<0;//开始播放+非暂停
	I2S_Play_Start();
} 
//关闭音频播放
void audio_stop(void)
{
	audiodev.status=0;
	I2S_Play_Stop();
}  
//得到path路径下,目标文件的总个数
//path:路径		    
//返回值:总有效文件数
u16 audio_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//临时目录
	FILINFO tfileinfo;	//临时文件信息		
	u8 *fn; 			 			   			     
    res=f_opendir(&tdir,(const TCHAR*)path); //打开目录
  	tfileinfo.lfsize=_MAX_LFN*2+1;						//长文件名最大长度
	tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);	//为长文件缓存区分配内存
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)//查询总的有效文件数
		{
	        res=f_readdir(&tdir,&tfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||tfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出		  
     		fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X40)//取高四位,看看是不是音乐文件	
			{
				rval++;//有效文件数增加1
			}	    
		}  
	} 
	myfree(SRAMIN,tfileinfo.lfname);
	return rval;
}
//显示曲目索引
//index:当前索引
//total:总文件数
void audio_index_show(u16 index,u16 total)
{
	//显示当前曲目的索引,及总曲目数
	LCD_ShowxNum(60+0,230,index,3,16,0X80);		//索引
	LCD_ShowChar(60+24,230,'/',16,0);
	LCD_ShowxNum(60+32,230,total,3,16,0X80); 	//总曲目				  	  
}
 
//显示播放时间,比特率 信息  
//totsec;音频文件总时间长度
//cursec:当前播放时间
//bitrate:比特率(位速)
void audio_msg_show(u32 totsec,u32 cursec,u32 bitrate)
{	
	static u16 playtime=0XFFFF;//播放时间标记	      
	if(playtime!=cursec)					//需要更新显示时间
	{
		playtime=cursec;
		//显示播放时间			 
		LCD_ShowxNum(60,210,playtime/60,2,16,0X80);		//分钟
		LCD_ShowChar(60+16,210,':',16,0);
		LCD_ShowxNum(60+24,210,playtime%60,2,16,0X80);	//秒钟		
 		LCD_ShowChar(60+40,210,'/',16,0); 	    	 
		//显示总时间    	   
 		LCD_ShowxNum(60+48,210,totsec/60,2,16,0X80);	//分钟
		LCD_ShowChar(60+64,210,':',16,0);
		LCD_ShowxNum(60+72,210,totsec%60,2,16,0X80);	//秒钟	  		    
		//显示位率			   
   		LCD_ShowxNum(60+110,210,bitrate/1000,4,16,0X80);//显示位率	 
		LCD_ShowString(60+110+32,210,200,16,16,"Kbps");	 
	} 		 
}
//播放音乐
void audio_play(void)
{
	u8 res;
 	DIR wavdir;	 		//目录
	FILINFO wavfileinfo;//文件信息
	u8 *fn;   			//长文件名
	u8 *pname;			//带路径的文件名
	u16 totwavnum; 		//音乐文件总数
	u16 curindex;		//图片当前索引
	u8 key;				//键值		  
 	u16 temp;
	u16 *wavindextbl;	//音乐索引表
	
	WM8978_ADDA_Cfg(1,0);	//开启DAC
	WM8978_Input_Cfg(0,0,0);//关闭输入通道
	WM8978_Output_Cfg(1,0);	//开启DAC输出  
	
 	while(f_opendir(&wavdir,"0:/MUSIC"))//打开音乐文件夹
 	{	    
		Show_Str(60,190,240,16,"MUSIC文件夹错误!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,190,240,206,WHITE);//清除显示	     
		delay_ms(200);				  
	} 									  
	totwavnum=audio_get_tnum("0:/MUSIC"); //得到总有效文件数
  	while(totwavnum==NULL)//音乐文件总数为0		
 	{	    
		Show_Str(60,190,240,16,"没有音乐文件!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,190,240,146,WHITE);//清除显示	     
		delay_ms(200);				  
	}										   
  	wavfileinfo.lfsize=_MAX_LFN*2+1;						//长文件名最大长度
	wavfileinfo.lfname=mymalloc(SRAMIN,wavfileinfo.lfsize);	//为长文件缓存区分配内存
 	pname=mymalloc(SRAMIN,wavfileinfo.lfsize);				//为带路径的文件名分配内存
 	wavindextbl=mymalloc(SRAMIN,2*totwavnum);				//申请2*totwavnum个字节的内存,用于存放音乐文件索引
 	while(wavfileinfo.lfname==NULL||pname==NULL||wavindextbl==NULL)//内存分配出错
 	{	    
		Show_Str(60,190,240,16,"内存分配失败!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,190,240,146,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	 
 	//记录索引
    res=f_opendir(&wavdir,"0:/MUSIC"); //打开目录
	if(res==FR_OK)
	{
		curindex=0;//当前索引为0
		while(1)//全部查询一遍
		{
			temp=wavdir.index;								//记录当前index
	        res=f_readdir(&wavdir,&wavfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||wavfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出		  
     		fn=(u8*)(*wavfileinfo.lfname?wavfileinfo.lfname:wavfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X40)//取高四位,看看是不是音乐文件	
			{
				wavindextbl[curindex]=temp;//记录索引
				curindex++;
			}	    
		} 
	}   
   	curindex=0;											//从0开始显示
   	res=f_opendir(&wavdir,(const TCHAR*)"0:/MUSIC"); 	//打开目录
	while(res==FR_OK)//打开成功
	{	
		dir_sdi(&wavdir,wavindextbl[curindex]);			//改变当前目录索引	   
        res=f_readdir(&wavdir,&wavfileinfo);       		//读取目录下的一个文件
        if(res!=FR_OK||wavfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出
     	fn=(u8*)(*wavfileinfo.lfname?wavfileinfo.lfname:wavfileinfo.fname);			 
		strcpy((char*)pname,"0:/MUSIC/");				//复制路径(目录)
		strcat((char*)pname,(const char*)fn);  			//将文件名接在后面
 		LCD_Fill(60,190,240,190+16,WHITE);				//清除之前的显示
		Show_Str(60,190,240-60,16,fn,16,0);				//显示歌曲名字 
		audio_index_show(curindex+1,totwavnum);
		key=audio_play_song(pname); 			 		//播放这个音频文件
		if(key==KEY2_PRES)		//上一曲
		{
			if(curindex)curindex--;
			else curindex=totwavnum-1;
 		}else if(key==KEY0_PRES)//下一曲
		{
			curindex++;		   	
			if(curindex>=totwavnum)curindex=0;//到末尾的时候,自动从头开始
 		}else break;	//产生了错误 	 
	} 											  
	myfree(SRAMIN,wavfileinfo.lfname);	//释放内存			    
	myfree(SRAMIN,pname);				//释放内存			    
	myfree(SRAMIN,wavindextbl);			//释放内存	 
} 
//播放某个音频文件
u8 audio_play_song(u8* fname)
{
	u8 res;  
	res=f_typetell(fname); 
	switch(res)
	{
		case T_WAV:
			res=wav_play_song(fname);
			break;
		default://其他文件,自动跳转到下一曲
			printf("can't play:%s\r\n",fname);
			res=KEY0_PRES;
			break;
	}
	return res;
}



























