#include "stm32f4xx.h" // Device header
#include "uart.h"
#include <stdio.h>
#include <string.h>
#include "delay.h"
#include "arm.h"


extern u8 USART3_RX_BUF[];

// 接收状态


/**
 * @brief 判断主字符串中是否包含子字符串
 *
 * @param main_string 主字符串
 * @param substring 子字符串
 * @return int 如果包含子字符串返回1，否则返回0
 */
int contains_substring(const u8 *main_string, const char *substring)
{
	// 如果strstr()返回非空指针，则表示找到了子字符串
	if (strstr((const char *)main_string, substring) != NULL)
	{
		return 1; // 找到子字符串
	}
	else
	{
		return 0; // 未找到子字符串
	}
}






