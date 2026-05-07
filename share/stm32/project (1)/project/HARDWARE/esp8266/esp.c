#include "stm32f4xx.h" // Device header
#include "uart.h"
#include <stdio.h>
#include <string.h>
#include "delay.h"
#include "esp.h"


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

/**
 * @brief 向ESP8266发送指令并等待响应
 *
 * @param str 要发送的指令
 * @param res 期望的响应
 * @return int 执行结果，始终返回0
 */
int esp_Cmd(const char *str, const char *res)
{
	while (0 == contains_substring(USART3_RX_BUF, res)) // 等待期望的响应
	{
		memset(USART3_RX_BUF, 0, sizeof(USART3_RX_BUF)); // 清空接收缓冲区
		printf("%s\r\n", str);						   // 发送指令
		delay_ms(2000);
		//delay_ms(8000); // 等待响应
	}
	memset(USART3_RX_BUF, 0, sizeof(USART3_RX_BUF)); // 清空接收缓冲区

	return 0;
}

/**
 * @brief 检查接收到的数据中是否包含特定的设备和属性。
 *
 * @param res1 设备名或关键字，用于识别设备。
 * @param res2 属性值，用于识别设备的属性状态。
 * @return int 如果接收到的数据中包含指定的设备和属性，则返回1，否则返回0。
 */
int esp_Get(const char *res1, const char *res2)
{
	// 示例接收数据格式："+MQTTSUBRECV:0,\"$sys/0S43la9qNI/LED/thing/property/set\",58,{\"id\":\"58\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":false}}"

	/* 检查接收缓冲区中是否包含指定的设备名或关键字 */
	if (1 == contains_substring(USART3_RX_BUF, res1))
	{
		/* 如果包含设备名，再检查是否包含指定的属性值 */
		if (1 == contains_substring(USART3_RX_BUF, res2))
		{
			return 1; // 找到匹配的设备和属性
		}
	}
	return 0; // 未找到匹配的设备和属性
}




