#include "videoplayer.h" 
#include "string.h"  
#include "key.h" 
#include "usart.h"   
#include "delay.h"
#include "timer.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "malloc.h"
#include "i2s.h" 
#include "wm8978.h" 
#include "mjpeg.h" 
#include "avi.h"
#include "exfuns.h"
#include "text.h"

#include "stepmotor.h"	
u8 a=0;   
extern u16 frame;
extern vu8 frameup;//视频播放时隙控制变量,当等于1的时候,可以更新下一帧视频


volatile u8 i2splaybuf;	//即将播放的音频帧缓冲编号
u8* i2sbuf[4]; 			//音频缓冲帧,共4帧,4*5K=20K
  
//音频数据I2S DMA传输回调函数
void audio_i2s_dma_callback(void) 
{      
	i2splaybuf++;
	if(i2splaybuf>3)i2splaybuf=0;
	if(DMA1_Stream4->CR&(1<<19))
	{	 
		DMA_MemoryTargetConfig(DMA1_Stream4,(u32)i2sbuf[i2splaybuf], DMA_Memory_0);
	}
	else 
	{  
    DMA_MemoryTargetConfig(DMA1_Stream4,(u32)i2sbuf[i2splaybuf], DMA_Memory_1); 		
	} 
} 
//得到path路径下,目标文件的总个数
//path:路径		    
//返回值:总有效文件数
u16 video_get_tnum(u8 *path)
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
			if((res&0XF0)==0X60)//取高四位,看看是不是视频文件	
			{
				rval++;//有效文件数增加1
			}	    
		}  
	} 
	myfree(SRAMIN,tfileinfo.lfname);
	return rval;
} 
//显示当前播放时间
//favi:当前播放的视频文件
//aviinfo;avi控制结构体
void video_time_show(FIL *favi,AVI_INFO *aviinfo)
{	 
	static u32 oldsec;	//上一次的播放时间
	u8* buf;
	u32 totsec=0;		//video文件总时间 
	u32 cursec; 		//当前播放时间 
	totsec=(aviinfo->SecPerFrame/1000)*aviinfo->TotalFrame;	//歌曲总长度(单位:ms) 
	totsec/=1000;		//秒钟数. 
  	cursec=((double)favi->fptr/favi->fsize)*totsec;//当前播放到多少秒了?  
	if(oldsec!=cursec)	//需要更新显示时间
	{
		buf=mymalloc(SRAMIN,100);//申请100字节内存
		oldsec=cursec; 
		POINT_COLOR=BLUE; 
		sprintf((char*)buf,"播放时间:%02d:%02d:%02d/%02d:%02d:%02d",cursec/3600,(cursec%3600)/60,cursec%60,totsec/3600,(totsec%3600)/60,totsec%60);
 		//Show_Str(10,90,lcddev.width-10,16,buf,16,0);	//显示歌曲名字
		myfree(SRAMIN,buf);		
	} 		 
}
//显示当前视频文件的相关信息 
//aviinfo;avi控制结构体
void video_info_show(AVI_INFO *aviinfo)
{	  
	u8 *buf;
	buf=mymalloc(SRAMIN,100);//申请100字节内存 
	POINT_COLOR=RED; 
	sprintf((char*)buf,"声道数:%d,采样率:%d",aviinfo->Channels,aviinfo->SampleRate*10); 
 	//Show_Str(10,50,lcddev.width-10,16,buf,16,0);	//显示歌曲名字
	sprintf((char*)buf,"帧率:%d帧",1000/(aviinfo->SecPerFrame/1000)); 
 	//Show_Str(10,70,lcddev.width-10,16,buf,16,0);	//显示歌曲名字
	myfree(SRAMIN,buf);	  
}
//视频基本信息显示
//name:视频名字
//index:当前索引
//total:总文件数
void video_bmsg_show(u8* name,u16 index,u16 total)
{		
	u8* buf;
	buf=mymalloc(SRAMIN,100);//申请100字节内存
	POINT_COLOR=RED;
	sprintf((char*)buf,"文件名:%s",name);
	//Show_Str(10,10,lcddev.width-10,16,buf,16,0);//显示文件名
	sprintf((char*)buf,"索引:%d/%d",index,total);	
	//Show_Str(10,30,lcddev.width-10,16,buf,16,0);//显示索引 		  	  
	myfree(SRAMIN,buf);		
}

//播放视频
void video_play(void)
{
	u8 res;
 	DIR vdir;	 		//目录
	FILINFO vfileinfo;	//文件信息
	u8 *fn;   			//长文件名
	u8 *pname;			//带路径的文件名
	u16 totavinum; 		//视频文件总数
	u16 curindex;		//视频文件当前索引
	u8 key;				//键值		  
 	u16 temp;
	u16 *vindextbl;		//视频文件索引表 

	
 	while(f_opendir(&vdir,"0:/VIDEO"))//打开视频文件夹
 	{	    
		Show_Str(60,190,240,16,"VIDEO文件夹错误!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,190,240,206,WHITE);//清除显示	     
		delay_ms(200);				  
	} 									  
	totavinum=video_get_tnum("0:/VIDEO"); //得到总有效文件数
  	while(totavinum==NULL)//视频文件总数为0		
 	{	    
		Show_Str(60,190,240,16,"没有视频文件!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,190,240,146,WHITE);//清除显示	     
		delay_ms(200);				  
	}										   
  	vfileinfo.lfsize=_MAX_LFN*2+1;						//长文件名最大长度
	vfileinfo.lfname=mymalloc(SRAMIN,vfileinfo.lfsize);	//为长文件缓存区分配内存
 	pname=mymalloc(SRAMIN,vfileinfo.lfsize);			//为带路径的文件名分配内存
 	vindextbl=mymalloc(SRAMIN,2*totavinum);				//申请2*totavinum个字节的内存,用于存放视频文件索引
 	while(vfileinfo.lfname==NULL||pname==NULL||vindextbl==NULL)//内存分配出错
 	{	    
		Show_Str(60,190,240,16,"内存分配失败!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,190,240,146,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	 
 	//记录索引
    res=f_opendir(&vdir,"0:/VIDEO"); //打开目录
	if(res==FR_OK)
	{
		curindex=0;//当前索引为0
		while(1)//全部查询一遍
		{
			temp=vdir.index;							//记录当前index
	        res=f_readdir(&vdir,&vfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||vfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出		  
     		fn=(u8*)(*vfileinfo.lfname?vfileinfo.lfname:vfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X60)//取高四位,看看是不是音乐文件	
			{
				vindextbl[curindex]=temp;//记录索引
				curindex++;
			}	    
		} 
	}   
   	curindex=0;										//从0开始显示
   	res=f_opendir(&vdir,(const TCHAR*)"0:/VIDEO"); 	//打开目录
	while(res==FR_OK)//打开成功
	{	
		dir_sdi(&vdir,vindextbl[curindex]);			//改变当前目录索引	   
        res=f_readdir(&vdir,&vfileinfo);       		//读取目录下的一个文件
        if(res!=FR_OK||vfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出
     	fn=(u8*)(*vfileinfo.lfname?vfileinfo.lfname:vfileinfo.fname);			 
		strcpy((char*)pname,"0:/VIDEO/");			//复制路径(目录)
		strcat((char*)pname,(const char*)fn);  		//将文件名接在后面 
		LCD_Clear(WHITE);							//先清屏
		//video_bmsg_show(fn,curindex+1,totavinum);	//显示名字,索引等信息		

		key=video_play_mjpeg(pname); 			 	//播放这个音频文件
		

		if(key==KEY2_PRES)		//上一曲
		{
			if(curindex)curindex--;
			else curindex=totavinum-1;
 		}else if(key==KEY0_PRES)//下一曲
		{
			curindex++;		   	
			if(curindex>=totavinum)curindex=0;//到末尾的时候,自动从头开始
 		}else break;	//产生了错误 	
		

		
	} 											  
	myfree(SRAMIN,vfileinfo.lfname);	//释放内存			    
	myfree(SRAMIN,pname);				//释放内存			    
	myfree(SRAMIN,vindextbl);			//释放内存	 
}
//播放一个mjpeg文件
//pname:文件名
//返回值:
//KEY0_PRES:下一曲
//KEY1_PRES:上一曲
//其他:错误
u8 video_play_mjpeg(u8 *pname)
{   
	u8* framebuf;	//视频解码buf	 
	u8* pbuf;		//buf指针  
	FIL *favi;
	u8  res=0;
	u16 offset=0; 
	u32	nr; 
	u8 key;   
    u8 i2ssavebuf;  
	i2sbuf[0]=mymalloc(SRAMIN,AVI_AUDIO_BUF_SIZE);	//申请音频内存
	i2sbuf[1]=mymalloc(SRAMIN,AVI_AUDIO_BUF_SIZE);	//申请音频内存
	i2sbuf[2]=mymalloc(SRAMIN,AVI_AUDIO_BUF_SIZE);	//申请音频内存
	i2sbuf[3]=mymalloc(SRAMIN,AVI_AUDIO_BUF_SIZE);	//申请音频内存 
	framebuf=mymalloc(SRAMIN,AVI_VIDEO_BUF_SIZE);	//申请视频buf
	favi=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//申请favi内存 
	memset(i2sbuf[0],0,AVI_AUDIO_BUF_SIZE);
	memset(i2sbuf[1],0,AVI_AUDIO_BUF_SIZE); 
	memset(i2sbuf[2],0,AVI_AUDIO_BUF_SIZE);
	memset(i2sbuf[3],0,AVI_AUDIO_BUF_SIZE); 
	if(i2sbuf[3]==NULL||framebuf==NULL||favi==NULL)
	{
		printf("memory error!\r\n");
		res=0XFF;
	}   
	while(res==0)
	{ 
		res=f_open(favi,(char *)pname,FA_READ);
		if(res==0)
		{
			pbuf=framebuf;			
			res=f_read(favi,pbuf,AVI_VIDEO_BUF_SIZE,&nr);//开始读取	
			if(res)
			{
				printf("fread error:%d\r\n",res);
				break;
			} 	 
			//开始avi解析
			res=avi_init(pbuf,AVI_VIDEO_BUF_SIZE);	//avi解析

			
			if(res)
			{
				printf("avi err:%d\r\n",res);
				break;
			} 	
			//video_info_show(&avix); 
			TIM6_Int_Init(avix.SecPerFrame/100-1,8400-1);//10Khz计数频率,加1是100us 
			offset=avi_srarch_id(pbuf,AVI_VIDEO_BUF_SIZE,"movi");//寻找movi ID	 
			avi_get_streaminfo(pbuf+offset+4);			//获取流信息 
			f_lseek(favi,offset+12);					//跳过标志ID,读地址偏移到流数据开始处	 
			res=mjpegdec_init((lcddev.width-avix.Width)/2,110+(lcddev.height-110-avix.Height)/2);//JPG解码初始化 
			if(avix.SampleRate)							//有音频信息,才初始化
			{
				WM8978_I2S_Cfg(2,0);	//飞利浦标准,16位数据长度
				I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16bextended);		//飞利浦标准,主机发送,时钟低电平有效,16位帧长度
				I2S2_SampleRate_Set(avix.SampleRate);	//设置采样率
				I2S2_TX_DMA_Init(i2sbuf[1],i2sbuf[2],avix.AudioBufSize/2); //配置DMA
				i2s_tx_callback=audio_i2s_dma_callback;	//回调函数指向I2S_DMA_Callback
				i2splaybuf=0;
				i2ssavebuf=0; 
				I2S_Play_Start(); //开启I2S播放 
			}
 			while(1)//播放循环
			{		
		
				if(avix.StreamID==AVI_VIDS_FLAG)	//视频流
				{
					pbuf=framebuf;
					f_read(favi,pbuf,avix.StreamSize+8,&nr);		//读入整帧+下一数据流ID信息  
					res=mjpegdec_decode(pbuf,avix.StreamSize);
					if(res)
					{
						printf("decode error!\r\n");
					} 
					while(frameup==0);	//等待时间到达(在TIM6的中断里面设置为1)
					frameup=0;			//标志清零
					frame++; 
				}else 	//音频流
				{		  
					//video_time_show(favi,&avix); 	//显示当前播放时间
					i2ssavebuf++;
					if(i2ssavebuf>3)i2ssavebuf=0;
					do
					{
						nr=i2splaybuf;
						if(nr)nr--;
						else nr=3; 
					}while(i2ssavebuf==nr);//碰撞等待. 
					f_read(favi,i2sbuf[i2ssavebuf],avix.StreamSize+8,&nr);//填充i2sbuf	 
					pbuf=i2sbuf[i2ssavebuf];  
				} 

				
				key=KEY_Scan(0);
				if(key==KEY0_PRES||key==KEY2_PRES)//KEY0/KEY2按下,播放下一个/上一个视频
				{
					res=key;
					break; 
				}else if(key==KEY1_PRES||key==WKUP_PRES)
				{
					I2S_Play_Stop();//关闭音频
					video_seek(favi,&avix,framebuf);
					pbuf=framebuf;
					I2S_Play_Start();//开启DMA播放 
				}


				if(end_ret() == 1) {            /////////////////*******************************************
					res = 100;
					break;		
				}
				
				if(avi_get_streaminfo(pbuf+avix.StreamSize))//读取下一帧 流标志
				{
					printf("frame error \r\n"); 
					res=KEY0_PRES;
					break; 
				} 					   	
			}
			I2S_Play_Stop();	//关闭音频
			TIM6->CR1&=~(1<<0); //关闭定时器6
			LCD_Set_Window(0,0,lcddev.width,lcddev.height);//恢复窗口
			mjpegdec_free();	//释放内存
			f_close(favi); 
		}
	}
	myfree(SRAMIN,i2sbuf[0]);
	myfree(SRAMIN,i2sbuf[1]);
	myfree(SRAMIN,i2sbuf[2]);
	myfree(SRAMIN,i2sbuf[3]);
	myfree(SRAMIN,framebuf);
	myfree(SRAMIN,favi);
	return res;
}
//avi文件查找
u8 video_seek(FIL *favi,AVI_INFO *aviinfo,u8 *mbuf)
{
	u32 fpos=favi->fptr;
	u8 *pbuf;
	u16 offset;
	u32 br;
	u32 delta;
	u32 totsec;
	u8 key; 
	totsec=(aviinfo->SecPerFrame/1000)*aviinfo->TotalFrame;	
	totsec/=1000;//秒钟数.
	delta=(favi->fsize/totsec)*5;		//每次前进5秒钟的数据量 
	while(1)
	{
		key=KEY_Scan(1); 
		if(key==WKUP_PRES)//快进
		{
			if(fpos<favi->fsize)fpos+=delta; 
			if(fpos>(favi->fsize-AVI_VIDEO_BUF_SIZE))
			{
				fpos=favi->fsize-AVI_VIDEO_BUF_SIZE;
			}
		}else if(key==KEY1_PRES)//快退
		{
			if(fpos>delta)fpos-=delta;
			else fpos=0; 
		}else break;
		f_lseek(favi,fpos);
		f_read(favi,mbuf,AVI_VIDEO_BUF_SIZE,&br);	//读入整帧+下一数据流ID信息 
		pbuf=mbuf; 
		if(fpos==0) //从0开始,得先寻找movi ID
		{
			offset=avi_srarch_id(pbuf,AVI_VIDEO_BUF_SIZE,"movi"); 
		}else offset=0;
		offset+=avi_srarch_id(pbuf+offset,AVI_VIDEO_BUF_SIZE,avix.VideoFLAG);	//寻找视频帧	
		avi_get_streaminfo(pbuf+offset);			//获取流信息 
		f_lseek(favi,fpos+offset+8);				//跳过标志ID,读地址偏移到流数据开始处	 
		if(avix.StreamID==AVI_VIDS_FLAG)
		{
			f_read(favi,mbuf,avix.StreamSize+8,&br);	//读入整帧 
			mjpegdec_decode(mbuf,avix.StreamSize); 		//显示视频帧
		}else 
		{
			printf("error flag");
		}
		//video_time_show(favi,&avix);  
	}
	return 0;
}






















