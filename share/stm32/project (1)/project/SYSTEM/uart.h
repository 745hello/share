#ifndef __UART_H__
#define __UART_H__
#include "stm32f4xx.h" 
#include <stdio.h>

#define USART3_REC_LEN  			80  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART3_RX_BUF[USART3_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART3_RX_STA;         		//接收状态标记	


void Usart1_Init(int baud);
void Usart2_Init(int baud);
void Usart3_Init(int baud);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);

void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num);

void Usart_SendString( USART_TypeDef * pUSARTx, char *str);

void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);

int fputc(int c,FILE *stream);


#endif

