#include "uart.h"

u8 USART3_RX_BUF[USART3_REC_LEN]; // 接收缓冲,最大USART3_REC_LEN个字节.
int i=0;
u16 USART3_RX_STA = 0; // 接收状态标记
/*
	串口1初始化
		PA9	---TX
		PA10 ---RX
		baud： 波特率
*/
void Usart1_Init(int baud)
{
	//1.定义USART+GPIO+NVIC外设的结构体
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
		
	//2.打开GPIOA端口的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
	//3.打开USART1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//4.选择USART1对应的引脚的复用功能
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	//5.配置USART1的引脚参数
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;				//复用模式
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;		//引脚速率
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;			//推挽输出	
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;				//上拉电阻
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9|GPIO_Pin_10;	//引脚编号
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//6.配置USART1的参数
	USART_InitStructure.USART_BaudRate 		= baud;										//通信速率 
	USART_InitStructure.USART_WordLength 	= USART_WordLength_8b;						//数据位8bit
	USART_InitStructure.USART_StopBits 		= USART_StopBits_1;							//停止位1bit
	USART_InitStructure.USART_Parity 		= USART_Parity_No;							//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
	USART_InitStructure.USART_Mode 			= USART_Mode_Rx | USART_Mode_Tx;			//收发模式
	USART_Init(USART1, &USART_InitStructure);
	
	//7.配置NVIC的参数
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			//中断编号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级，最高优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//打开中断通道
	NVIC_Init(&NVIC_InitStructure);
	
	//8.设置中断的触发方式  接收到数据触发
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	//9.使能USART1
	USART_Cmd(USART1, ENABLE);

}


/*
	串口2初始化
		PA2	---TX
		PA3 ---RX
		baud： 波特率
*/
void Usart2_Init(int baud)
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//打开中断通道
	NVIC_Init(&NVIC_InitStructure);
	
	//8.设置中断的触发方式  接收到数据触发
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	//9.使能USART2
	USART_Cmd(USART2, ENABLE);

}


/*
	串口3初始化
		PB10	---TX
		PB11 	---RX
		baud： 波特率
*/
void Usart3_Init(int baud)
{
	//1.定义USART+GPIO+NVIC外设的结构体
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
		
	//2.打开GPIOB端口的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  
	//3.打开USART3的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	//4.选择USART3对应的引脚的复用功能
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
	
	//5.配置USART3的引脚参数
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;				//复用模式
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;		//引脚速率
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;			//推挽输出	
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;				//上拉
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_11|GPIO_Pin_10;	//引脚编号
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//6.配置USART3的参数
	USART_InitStructure.USART_BaudRate 		= baud;										//通信速率
	USART_InitStructure.USART_WordLength 	= USART_WordLength_8b;						//数据位8bit
	USART_InitStructure.USART_StopBits 		= USART_StopBits_1;							//停止位1bit
	USART_InitStructure.USART_Parity 		= USART_Parity_No;							//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
	USART_InitStructure.USART_Mode 			= USART_Mode_Rx | USART_Mode_Tx;			//收发模式
	USART_Init(USART3, &USART_InitStructure);
	
	//7.配置NVIC的参数
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			//中断编号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//打开中断通道
	NVIC_Init(&NVIC_InitStructure);
	
	//8.设置中断的触发方式  接收到数据触发
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	//9.使能USART1
	USART_Cmd(USART3, ENABLE);

}



//串口1的中断服务函数
void USART1_IRQHandler(void)
{
	//1.判断USART1是否接收到数据
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		
		
		USART_ClearITPendingBit(USART1,USART_IT_RXNE); //清除中断标志
	}
}

void USART3_IRQHandler(void) // 串口3中断服务程序
{

	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // 检查接收中断标志
	{
		u8 Res = USART_ReceiveData(USART3); // 读取接收到的数据

			// 检查缓冲区是否已满
			if (i < (80 - 1))
			{
				USART3_RX_BUF[i] = Res; // 将接收到的数据存储在缓冲区中
				i++;					  // 增加接收状态计数器

				// 检查是否接收到结束字符（例如换行符 '\n'）
				if (Res == '\n')
				{
					 USART3_RX_BUF[i]  = '\0';
			     USART3_RX_STA = 1;
					 i=0;
				}
			}
			else
			{
				  // 缓冲区溢出，重置状态
				  USART3_RX_STA = 0;
				  i=0;
			}
		
		// 清除接收中断标志，以避免重复进入中断
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}

/****************** 发送元素个数为num、元素类型为char的数组*******************/
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
	uint8_t i;
	
	for(i=0; i<num; i++)
	{
		/* 发送一个字节数据到USART */
		Usart_SendByte(pUSARTx,array[i]);	

	}
	/* 等待发送完成 */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}

/*****************  发送字符串 **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}

/*****************  发送一个16位数 **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* 取出高八位 */
	temp_h = (ch&0XFF00)>>8;
	/* 取出低八位 */
	temp_l = ch&0XFF;
	
	/* 发送高八位 */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* 发送低八位 */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}







#if 1
#pragma import(__use_no_semihosting)
// 标准库需要的支持函数
struct __FILE
{
	int handle;
};

FILE __stdout;

// 重定义fputc函数
int fputc(int ch, FILE *f)
{
	while ((USART1->SR & 0X40) == 0)
		; // 循环发送,直到发送完毕
	USART1->DR = (u8)ch;
	return ch;
}
#endif





