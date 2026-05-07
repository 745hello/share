/************************************************
神舟号 STM32F407开发板 
安徽中科康芯科技有限公司  
************************************************/
#include "MKS141.h"
uint8_t finish_Flag=0;
uint8_t USART2_BUF[USART2_LEN]; //接收缓冲,最大USART_REC_LEN个字节.
uint8_t USART2_STA=0;       //接收状态标记

/*
	串口2初始化
		PA2	---TX	---> 传感器 RXD
		PA3 ---RX	---> 传感器 TXD
		baud： 波特率
*/
void MKS141_Usart2_Init(int baud)
{
	//1.定义USART+GPIO+NVIC外设的结构体
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
		
	//2.打开GPIOA端口的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
	//3.打开USART2的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//4.选择USART2对应的引脚的复用功能
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	
	//5.配置USART2的引脚参数
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;				//复用模式
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;		//引脚速率
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;			//推挽输出	
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;				//上拉
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_2|GPIO_Pin_3;	//引脚编号
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//6.配置USART2的参数
	USART_InitStructure.USART_BaudRate 		= baud;										//通信速率
	USART_InitStructure.USART_WordLength 	= USART_WordLength_8b;						//数据位8bit
	USART_InitStructure.USART_StopBits 		= USART_StopBits_1;							//停止位1bit
	USART_InitStructure.USART_Parity 		= USART_Parity_No;							//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
	USART_InitStructure.USART_Mode 			= USART_Mode_Rx | USART_Mode_Tx;			//收发模式
	USART_Init(USART2, &USART_InitStructure);
	
	//7.配置NVIC的参数
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			//中断编号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//打开中断通道
	NVIC_Init(&NVIC_InitStructure);
	
	//8.设置中断的触发方式  接收到数据触发
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	//9.使能USART2
	USART_Cmd(USART2, ENABLE);
	
	Usart_SendByte(USART2,0x8a);
}
MKS141_Date Get_MKS141_Date(void)
{
	MKS141_Date mk;
	mk.Head = USART2_BUF[0];
	mk.HR = USART2_BUF[65];
	mk.SPO2 = USART2_BUF[66]; //血氧
	mk.SBP = USART2_BUF[71];
	mk.DBP = USART2_BUF[72];
	return mk;
}

void USART2_IRQHandler(void)                	//串口2中断服务程序
{
	uint8_t Res;
	static u8 flag;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		//printf("qq\n");
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		Res =USART_ReceiveData(USART2);					//读取接收到的数据
		if (Res==(0xff)) 								//当数据包的首字节为0xff,则继续接收,否则丢弃  
		{
			flag = 1;
		}
		if(flag)
		{
			USART2_BUF[USART2_STA]=Res;					//每次进中断在这里赋值给数组
			USART2_STA++;																		
		}
		if(USART2_STA>(USART2_LEN-1))					//达到76字节时,回到数组第1位
		{
			USART2_STA=0;
			flag= 0;
			finish_Flag=1;
		}		
	} 
} 

void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);	
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}


