#include "common.h"

#define REC_CMD  "arecord -d3 -c1 -r16000 -traw -fS16_LE cmd.pcm"
#define PCM_FILE "./cmd.pcm"
#define DEV_PATH2   "/dev/ttySAC2"//串口2的驱动路径

void showbitmap(bitmap *bm, int x0, int y0);
void font_show(char *text,int pixelSize,int outFrameWidth,int outFrameHeight,int outFrameColor,int fontPosX,int fontPosY,int fontColor,int frameToLcdPosX,int frameToLcdPosY);
int get_voice_info(int sockfd);
void serial_init();
int get_stm32_data(char *stm_cmd);
extern unsigned char *lcd;
int  tty2_fd ;
//启动界面
void start(void)
{
	int tx,ty;
	//显示启动界面
	show_bmp("/ai_bmp/start.bmp",0,0);
	//检测启动按钮
	while(1)
	{
		get_xy(&tx, &ty);
		if(tx>320&&tx<500&&ty>330&&ty<390)
		{
			break;
		}
	}
}
//ai语音助手界面
void ai_voice(int sockfd)
{
	int tx,ty;//触屏坐标
	int n=0;//获取的数据
	char font_data[1024] = {0};//屏幕显示文字：传感器数据+ai互动数据 
	//显示ai界面
	show_bmp("ai_bmp/voice.bmp",0,0);

	//检测语音按钮
	while(1)
	{
		get_xy(&tx, &ty);
		if(tx>340&&tx<430&&ty>360&&ty<450)
		{
			int cmd = get_voice_info(sockfd);
			if(cmd==0)
			{
				printf("我好像没听懂请再说一遍\n");
			}
			if(cmd == 999)
			{
				printf("byebye\n");
				break;
			}
			if(cmd==2)
			{
				printf("hello\n");
				font_show(	"你好吖",
									32,//中文64*64  数字、英文 64*32
								   700,
								   300,
							0xffffff00,// 0x00ff0000  	R00 Gff B00 A00--》绿色
									10,
								    80,
							0xff000000,//白色
								   36,
								   26);
				//播放准备好的音频文件
			    system("aplay /ai_wav/nihao.wav&");
			}
			if(cmd == 3)//量血压--收缩压
			{
				//printf("请把手放到传感器,等待一会\n");
				
				font_show(	"请把手放到传感器,等待一会",
									32,//中文64*64  数字、英文 64*32
								   700,
								   300,
							0xffffff00,// 0x00ff0000  	R00 Gff B00 A00--》绿色
									10,
								    80,
							0xff000000,//白色
								   36,
								   26);
				 system("aplay /ai_wav/dengdai.wav");
				//获取血压数据--收缩压
				n = get_stm32_data("mks-sbp\n");
			
				//分析数据
				if(n<139)
				{   
			        bzero(font_data,1024);
					printf("血压正常%d\n",n);
					sprintf(font_data,"血压：%d  你的血压正常，继续保持哦",n);
				    font_show(	font_data,
									32,//中文64*64  数字、英文 64*32
								   700,
								   300,
							0xffffff00,// 0x00ff0000  	R00 Gff B00 A00--》绿色
									10,
								    80,
							0xff000000,//白色
								   36,
								   26);
					system("aplay /ai_wav/xueya.wav");
				}
                if(n >139)//血压过高
                {
					
					printf("血压%d \n",n);	
				}				
			}
			if(cmd == 6)//我要测心率
			{
				
				
			}
			
		}
	}
	
	
	
}
int main(int argc, char const *argv[]) //./voicectl  ubuntu-IP
{
	//执行参数检测
	
	if(argc != 2)
	{
		printf("Usage: %s <ubuntu-IP>\n", argv[0]);
		exit(0);
	}
	//硬件初始化
	lcd_init();//显示屏
	ts_init();//触摸屏
	serial_init();//串口--stm32
	//与语音识别引擎建立连接
	int sockfd = init_sock(argv[1]); //由命令行传入一个对方的IP 等效于socket+bind+listen+accept	
	
	//启动界面
	start();	
	printf("waiting touch...\n");	
aa:
	//ai语音界面
	ai_voice(sockfd);
	goto aa;	
	
	close(sockfd);
	return 0;
}
//获取stm32传感器数据
int get_stm32_data(char *stm_cmd)
{
	char stm_data[1024]={0};
	//获取血压数据--收缩压
	if(strstr(stm_cmd,"mks-sbp"))
	{
		printf("send mks cmd\n");
		int len = strlen(stm_cmd);
		printf("len %d\n",len);
		write(tty2_fd,stm_cmd,len);
		sleep(3);
		while(1)
		{
			bzero(stm_data,1024);
			read(tty2_fd,stm_data,1024);
			
			printf("sbp %s\n",stm_data);
			
			if(atoi(stm_data)>0)
			    return atoi(stm_data);
		}
		
	}
		//获取血压数据--心率
	if(strstr(stm_cmd,"mks-hr"))
	{
		printf("send mks cmd\n");
		int len = strlen(stm_cmd);
		printf("len %d\n",len);
		write(tty2_fd,stm_cmd,len);
		sleep(3);
		while(1)
		{
			bzero(stm_data,1024);
			read(tty2_fd,stm_data,1024);
			
			printf("hr %s\n",stm_data);
			
			if(atoi(stm_data)>0)
			    return atoi(stm_data);
		}
		
	}
	//获取温度
	if(strstr(stm_cmd,"asm"))
	{
		write(tty2_fd,stm_cmd,strlen(stm_cmd));
		sleep(3);
		while(1)
		{
			bzero(stm_data,1024);
			read(tty2_fd,stm_data,1024);
			
			printf("asm %s\n",stm_data);
			
			if(atoi(stm_data)>32)
			    return atoi(stm_data);
		}
		
	}
	return 0;
}
//arm-stm32串口初始化
void serial_init()
{
	//打开arm平台串口
	tty2_fd = open(DEV_PATH2, O_RDWR | O_NOCTTY);
	
	//配置串口属性--默认波特率115200
	init_tty(tty2_fd);
	
	
}
//语音识别---返回识别的语句id
int get_voice_info(int sockfd)
{
	int id_num=0,num;
    font_show(	"我在听，请说出你的需求",
									32,//中文64*64  数字、英文 64*32
								   700,
								   300,
							0xffffff00,// 0x00ff0000  	R00 Gff B00 A00--》绿色
									10,
								    80,
							0xff000000,//白色
								   36,
								   26);
		
		//启动录音
		system(REC_CMD);

		// 2，将录制好的PCM音频发送给语音识别引擎
		send_pcm(sockfd, PCM_FILE);

		// 3，等待对方回送识别结果（字符串ID）
		xmlChar *id = wait4id(sockfd);
		if(id == NULL)
			return 0;
		
		//将字符串的id转化成为整形的id
		id_num=atoi((char *)id);
		
		printf("id: %d\n",id_num);
	    return id_num;
	
}
void showbitmap(bitmap *bm, int x0, int y0)
{
    //printf("width:%d, height:%d, byteperpixel:%d\n", bm->width, bm->height, bm->byteperpixel);
	int x,y;
	int bgrIndex;
	for(y=y0; y<bm->height+y0; y++)
	{
		for(x=x0;x<bm->width+x0; x++)
		{
			bgrIndex = ((y-y0) * bm->width + (x-x0)) * 4;
			lcd[y*800*4+x*4+0] = bm->map[bgrIndex + 0];//B
	        lcd[y*800*4+x*4+1] = bm->map[bgrIndex + 1];//G
	        lcd[y*800*4+x*4+2] = bm->map[bgrIndex + 2];//R
	        lcd[y*800*4+x*4+3] = bm->map[bgrIndex + 3];//A			
		}
		
	}
} 

/*
text:你要显示的文本
pixelSize:字体的大小  最大为72
outFrameWidth:输出框的宽度
outFrameHeight:输出框的高度
outFrameColor:输出框的背景颜色
fontPosX:文本相对于输出框的位置 X
fontPosY:文本相对于输出框的位置 Y
fontColor：文本的颜色
frameToLcdPosX:输出框相对于液晶屏的位置坐标X
frameToLcdPosY:输出框相对于液晶屏的位置坐标Y
*/
void font_show(char *text,int pixelSize,int outFrameWidth,int outFrameHeight,int outFrameColor,int fontPosX,int fontPosY,int fontColor,int frameToLcdPosX,int frameToLcdPosY)
{
	//1.初始化字库 
    // 注意要先将ttf文件放入开发板上
    font *f = fontLoad("/simfang.ttf"); // 指定字库文件，比如simfang.ttf

    //2.设置字体的大小 
    fontSetSize(f, pixelSize);

    //3.设置字体输出框的大小
    bitmap *bm;
    bm = createBitmapWithInit(outFrameWidth, outFrameHeight, 4, outFrameColor);

    //4.把字体输出到输出框中
    fontPrint(f, bm, fontPosX, fontPosY, text, fontColor, 0);

    //5.将bm妥善地放置到LCD上显示出来
    showbitmap(bm, frameToLcdPosX, frameToLcdPosY);

	// 关闭bitmap
	destroyBitmap(bm);
   
	// 关闭字体库
	fontUnload(f);
}