实验器材:
	探索者STM32F4开发板
	
实验目的:
	学习在STM32上面的图片显示方法,实现一个简单的数码相框.
	
硬件资源:
	1,DS0(连接在PF9),DS1(连接在PF10)
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面)
	3,ALIENTEK 2.8/3.5/4.3/7寸TFTLCD模块(通过FSMC驱动,FSMC_NE4接LCD片选/A6接RS)
	4,按键KEY0(PE4)/KEY2(PE2)/KEY_UP(PA0,也称之为WK_UP)
	5,SD卡,通过SDIO(SDIO_D0~D4(PC8~PC11),SDIO_SCK(PC12),SDIO_CMD(PD2))连接 
	6,W25Q128(SPI FLASH芯片,连接在SPI1上)

实验现象:
	本实验开机的时候先检测字库，然后检测SD卡是否存在，如果SD卡存在，则开始查找SD卡根目录下的PICTURE文件夹，
	如果找到则显示该文件夹下面的图片文件（支持bmp、jpg、jpeg或gif格式），循环显示，通过按KEY0和KEY2可以快
	速浏览下一张和上一张，KEY_UP按键用于暂停/继续播放，DS1用于指示当前是否处于暂停状态。如果未找到PICTURE
	文件夹/任何图片文件，则提示错误。同样我们也是用DS0来指示程序正在运行。
	本实验也可以通过USMART调用ai_load_picfile和minibmp_decode解码任意指定路径的图片. 
   
注意事项: 
	1,4.3寸和7寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).
	2,本例程在LCD_Init函数里面(在ILI93xx.c),用到了printf,如果不初始化串口1,将导致液晶无法显示!!
	3,该实验须自备SD卡一个，并在SD卡根目录新建：PICTURE文件夹，并放入一些图片（BMP/JPG/JPEG/GIF）。    
	4,如果有些jpg/jpeg格式不能显示的话，用XP自带的画图工具打开，再保存一下，一般就可以显示了。
	5,JPEG/JPG/BMP图片，程序会根据LCD分辨率自动缩放，以适应LCD，而GIF则必须小于屏幕分辨率才可以显示。 


					正点原子@ALIENTEK
					2014-10-25
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com
					
					
					
					
					
					
					
					
					
					
					
					
					
					
					
					
					