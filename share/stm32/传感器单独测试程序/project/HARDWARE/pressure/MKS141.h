#ifndef __MKS141_H__
#define __MKS141_H__
#include "stm32f4xx.h"
#include <stdio.h>

typedef struct MKS141
{
	uint8_t Head; //数据帧头，0xff代表获取成功
	uint8_t HR; //heart rate 心率
	uint8_t SBP; //收缩压
	uint8_t DBP; //舒张压
	uint8_t SPO2; //血氧
}MKS141_Date;



#define USART2_LEN  			88  	//定义最大接收字节数 
extern uint8_t finish_Flag;
extern uint8_t USART2_BUF[USART2_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

void MKS141_Usart2_Init(int baud);

MKS141_Date Get_MKS141_Date(void);

void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);

#endif
