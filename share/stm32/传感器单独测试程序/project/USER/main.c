/************************************************
 神舟号 STM32F407开发板 
 安徽中科康芯科技有限公司  
************************************************/
#include "main.h"
extern u8 USART3_RX_BUF[];

extern u16 USART3_RX_STA;
void Program_Init(void)
{
	/*抢占优先级2位，响应优先级1位*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//Exti4_Init();
	Usart1_Init(9600); //在串口助手上用printf打印数据需要调用此函数	
	AS6221_IIC_Init(); //温度传感器初始化
	MKS141_Usart2_Init(38400); //心率血氧血压综合检测模块，只能是38400

	Usart3_Init(115200); //通信串口---与arm平台通信
	
}

//程序的入口
// 主函数
// 主函数示例
int main(void)
{
	
  int temp;
	MKS141_Date mk;
	Program_Init();
	 
	while(1)
	{

			 mk = Get_MKS141_Date(); //获取心率 血压 血氧 
			temp=(int)AS6221_GetTemperature()	;		    
		   if(mk.HR)
		   {
				 printf("hert rate: %d\r\n",mk.HR);//心率
		 		   
		 		  
		   }
       if(mk.SBP)
		   {
				 printf("sbp: %d\n",mk.SBP);//血压--收缩压 
		 		   
		   }	
			 if(temp > 32)
			 {
			    printf("temp %d\r\n",temp+2);
			 }		 
		   printf("-\r\n");
	     delay_s(1);
	}
   
	
}






