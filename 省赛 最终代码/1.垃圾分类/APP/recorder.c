#include "recorder.h" 
#include "audioplay.h"
#include "ff.h"
#include "malloc.h"
#include "text.h"
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
//录音机 应用代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/6/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
  
u8 *i2srecbuf1;
u8 *i2srecbuf2; 

FIL* f_rec=0;		//录音文件	
u32 wavsize;		//wav数据大小(字节数,不包括文件头!!)
u8 rec_sta=0;		//录音状态
					//[7]:0,没有开启录音;1,已经开启录音;
					//[6:1]:保留
					//[0]:0,正在录音;1,暂停录音;
					
//录音 I2S_DMA接收中断服务函数.在中断里面写入数据
void rec_i2s_dma_rx_callback(void) 
{    
	u16 bw;
	u8 res;
	if(rec_sta==0X80)//录音模式
	{  
		if(DMA1_Stream3->CR&(1<<19))
		{
			res=f_write(f_rec,i2srecbuf1,I2S_RX_DMA_BUF_SIZE,(UINT*)&bw);//写入文件
			if(res)
			{
				printf("write error:%d\r\n",res);
			}
			 
		}else 
		{
			res=f_write(f_rec,i2srecbuf2,I2S_RX_DMA_BUF_SIZE,(UINT*)&bw);//写入文件
			if(res)
			{
				printf("write error:%d\r\n",res);
			}
		}
		wavsize+=I2S_RX_DMA_BUF_SIZE;
	} 
}  
const u16 i2splaybuf[2]={0X0000,0X0000};//2个16位数据,用于录音时I2S Master发送.循环发送0.
//进入PCM 录音模式 		  
void recoder_enter_rec_mode(void)
{
	WM8978_ADDA_Cfg(0,1);		//开启ADC
	WM8978_Input_Cfg(1,1,0);	//开启输入通道(MIC&LINE IN)
	WM8978_Output_Cfg(0,1);		//开启BYPASS输出 
	WM8978_MIC_Gain(46);		//MIC增益设置 
	
	WM8978_I2S_Cfg(2,0);		//飞利浦标准,16位数据长度
	I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);			//飞利浦标准,主机发送,时钟低电平有效,16位帧长度 
	I2S2ext_Init(I2S_Standard_Phillips,I2S_Mode_SlaveRx,I2S_CPOL_Low,I2S_DataFormat_16b);		//飞利浦标准,从机接收,时钟低电平有效,16位帧长度	
	I2S2_SampleRate_Set(16000);	//设置采样率 
 	I2S2_TX_DMA_Init((u8*)&i2splaybuf[0],(u8*)&i2splaybuf[1],1); 		//配置TX DMA 
	DMA1_Stream4->CR&=~(1<<4);	//关闭传输完成中断(这里不用中断送数据) 
	I2S2ext_RX_DMA_Init(i2srecbuf1,i2srecbuf2,I2S_RX_DMA_BUF_SIZE/2); 	//配置RX DMA
  	i2s_rx_callback=rec_i2s_dma_rx_callback;//回调函数指wav_i2s_dma_callback
 	I2S_Play_Start();	//开始I2S数据发送(主机)
	I2S_Rec_Start(); 	//开始I2S数据接收(从机)
	recoder_remindmsg_show(0);
}  
//进入PCM 放音模式 		  
void recoder_enter_play_mode(void)
{
	WM8978_ADDA_Cfg(1,0);		//开启DAC 
	WM8978_Input_Cfg(0,0,0);	//关闭输入通道(MIC&LINE IN)
	WM8978_Output_Cfg(1,0);		//开启DAC输出 
	WM8978_MIC_Gain(0);			//MIC增益设置为0 
	I2S_Play_Stop();			//停止时钟发送
	I2S_Rec_Stop(); 			//停止录音
	recoder_remindmsg_show(1);
}
//初始化WAV头.
void recoder_wav_init(__WaveHeader* wavhead) //初始化WAV头			   
{
	wavhead->riff.ChunkID=0X46464952;	//"RIFF"
	wavhead->riff.ChunkSize=0;			//还未确定,最后需要计算
	wavhead->riff.Format=0X45564157; 	//"WAVE"
	wavhead->fmt.ChunkID=0X20746D66; 	//"fmt "
	wavhead->fmt.ChunkSize=16; 			//大小为16个字节
	wavhead->fmt.AudioFormat=0X01; 		//0X01,表示PCM;0X01,表示IMA ADPCM
 	wavhead->fmt.NumOfChannels=2;		//双声道
 	wavhead->fmt.SampleRate=16000;		//16Khz采样率 采样速率
 	wavhead->fmt.ByteRate=wavhead->fmt.SampleRate*4;//字节速率=采样率*通道数*(ADC位数/8)
 	wavhead->fmt.BlockAlign=4;			//块大小=通道数*(ADC位数/8)
 	wavhead->fmt.BitsPerSample=16;		//16位PCM
   	wavhead->data.ChunkID=0X61746164;	//"data"
 	wavhead->data.ChunkSize=0;			//数据大小,还需要计算  
} 						    
//显示录音时间和码率
//tsec:秒钟数.
void recoder_msg_show(u32 tsec,u32 kbps)
{   
	//显示录音时间			 
	LCD_ShowString(30,210,200,16,16,"TIME:");	  	  
	LCD_ShowxNum(30+40,210,tsec/60,2,16,0X80);	//分钟
	LCD_ShowChar(30+56,210,':',16,0);
	LCD_ShowxNum(30+64,210,tsec%60,2,16,0X80);	//秒钟	
	//显示码率		 
	LCD_ShowString(140,210,200,16,16,"KPBS:");	  	  
	LCD_ShowxNum(140+40,210,kbps/1000,4,16,0X80);	//码率显示 	
}  	
//提示信息
//mode:0,录音模式;1,放音模式
void recoder_remindmsg_show(u8 mode)
{
	LCD_Fill(30,120,lcddev.width,180,WHITE);//清除原来的显示
	POINT_COLOR=RED;
	if(mode==0)	//录音模式
	{
		Show_Str(30,120,200,16,"KEY0:REC/PAUSE",16,0); 
		Show_Str(30,140,200,16,"KEY2:STOP&SAVE",16,0); 
		Show_Str(30,160,200,16,"WK_UP:PLAY",16,0); 
	}else		//放音模式
	{
		Show_Str(30,120,200,16,"KEY0:STOP Play",16,0);  
		Show_Str(30,140,200,16,"WK_UP:PLAY/PAUSE",16,0); 
	}
}
//通过时间获取文件名
//仅限在SD卡保存,不支持FLASH DISK保存
//组合成:形如"0:RECORDER/REC20120321210633.wav"的文件名
void recoder_new_pathname(u8 *pname)
{	 
	u8 res;					 
	u16 index=0;
	while(index<0XFFFF)
	{
		sprintf((char*)pname,"0:RECORDER/REC%05d.wav",index);
		res=f_open(ftemp,(const TCHAR*)pname,FA_READ);//尝试打开这个文件
		if(res==FR_NO_FILE)break;		//该文件名不存在=正是我们需要的.
		index++;
	}
} 
//WAV录音 
void wav_recorder(void)
{ 
	u8 res;
	u8 key;
	u8 rval=0;
	__WaveHeader *wavhead=0; 
 	DIR recdir;	 					//目录  
 	u8 *pname=0;
	u8 timecnt=0;					//计时器   
	u32 recsec=0;					//录音时间 
  	while(f_opendir(&recdir,"0:/RECORDER"))//打开录音文件夹
 	{	 
		Show_Str(30,230,240,16,"RECORDER文件夹错误!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,230,240,246,WHITE);		//清除显示	     
		delay_ms(200);				  
		f_mkdir("0:/RECORDER");				//创建该目录   
	}   
	i2srecbuf1=mymalloc(SRAMIN,I2S_RX_DMA_BUF_SIZE);//I2S录音内存1申请
	i2srecbuf2=mymalloc(SRAMIN,I2S_RX_DMA_BUF_SIZE);//I2S录音内存2申请  
  	f_rec=(FIL *)mymalloc(SRAMIN,sizeof(FIL));		//开辟FIL字节的内存区域  
 	wavhead=(__WaveHeader*)mymalloc(SRAMIN,sizeof(__WaveHeader));//开辟__WaveHeader字节的内存区域 
	pname=mymalloc(SRAMIN,30);						//申请30个字节内存,类似"0:RECORDER/REC00001.wav" 
	if(!i2srecbuf1||!i2srecbuf2||!f_rec||!wavhead||!pname)rval=1; 	if(rval==0)		
	{
		recoder_enter_rec_mode();	//进入录音模式,此时耳机可以听到咪头采集到的音频   
		pname[0]=0;					//pname没有任何文件名 
 	   	while(rval==0)
		{
			key=KEY_Scan(0);
			switch(key)
			{		
				case KEY2_PRES:	//STOP&SAVE
					if(rec_sta&0X80)//有录音
					{
						rec_sta=0;	//关闭录音
						wavhead->riff.ChunkSize=wavsize+36;		//整个文件的大小-8;
				   		wavhead->data.ChunkSize=wavsize;		//数据大小
						f_lseek(f_rec,0);						//偏移到文件头.
				  		f_write(f_rec,(const void*)wavhead,sizeof(__WaveHeader),&bw);//写入头数据
						f_close(f_rec);
						wavsize=0;
					}
					rec_sta=0;
					recsec=0;
				 	LED1=1;	 						//关闭DS1
					LCD_Fill(30,190,lcddev.width,lcddev.height,WHITE);//清除显示,清除之前显示的录音文件名		      
					break;	 
				case KEY0_PRES:	//REC/PAUSE
					if(rec_sta&0X01)//原来是暂停,继续录音
					{
						rec_sta&=0XFE;//取消暂停
					}else if(rec_sta&0X80)//已经在录音了,暂停
					{
						rec_sta|=0X01;	//暂停
					}else				//还没开始录音 
					{
						recsec=0;	 
						recoder_new_pathname(pname);			//得到新的名字
						Show_Str(30,190,lcddev.width,16,"录制:",16,0);		   
						Show_Str(30+40,190,lcddev.width,16,pname+11,16,0);//显示当前录音文件名字
				 		recoder_wav_init(wavhead);				//初始化wav数据	
	 					res=f_open(f_rec,(const TCHAR*)pname, FA_CREATE_ALWAYS | FA_WRITE); 
						if(res)			//文件创建失败
						{
							rec_sta=0;	//创建文件失败,不能录音
							rval=0XFE;	//提示是否存在SD卡
						}else 
						{
							res=f_write(f_rec,(const void*)wavhead,sizeof(__WaveHeader),&bw);//写入头数据
							recoder_msg_show(0,0);
 							rec_sta|=0X80;	//开始录音	 
						} 
 					}
					if(rec_sta&0X01)LED1=0;	//提示正在暂停
					else LED1=1;
					break;  
				case WKUP_PRES:	//播放最近一段录音
					if(rec_sta!=0X80)//没有在录音
					{   	 		 				  
						if(pname[0])//如果触摸按键被按下,且pname不为空
						{				 
							Show_Str(30,190,lcddev.width,16,"播放:",16,0);		   
							Show_Str(30+40,190,lcddev.width,16,pname+11,16,0);//显示当播放的文件名字
							recoder_enter_play_mode();	//进入播放模式
							audio_play_song(pname);		//播放pname
							LCD_Fill(30,190,lcddev.width,lcddev.height,WHITE);//清除显示,清除之前显示的录音文件名	  
							recoder_enter_rec_mode();	//重新进入录音模式 
						}
					}
					break;
			} 
			delay_ms(5);
			timecnt++;
			if((timecnt%20)==0)LED0=!LED0;//DS0闪烁  
 			if(recsec!=(wavsize/wavhead->fmt.ByteRate))	//录音时间显示
			{	   
				LED0=!LED0;//DS0闪烁 
				recsec=wavsize/wavhead->fmt.ByteRate;	//录音时间
				recoder_msg_show(recsec,wavhead->fmt.SampleRate*wavhead->fmt.NumOfChannels*wavhead->fmt.BitsPerSample);//显示码率
			}
		}		 
	}    
	myfree(SRAMIN,i2srecbuf1);	//释放内存
	myfree(SRAMIN,i2srecbuf2);	//释放内存  
	myfree(SRAMIN,f_rec);		//释放内存
	myfree(SRAMIN,wavhead);		//释放内存  
	myfree(SRAMIN,pname);		//释放内存  
}



































