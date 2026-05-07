#include "delay.h"
//延时微秒
void delay_us(uint32_t nus)
{
	SysTick->CTRL = 0; 			// 关闭定时器
	SysTick->LOAD = nus*21 - 1; // 指的是延时时间对应的计数次数 			
	SysTick->VAL = 0; 			// 清空当前计数值
	SysTick->CTRL = 1; 			// 开启定时器  并且时钟频率是21MHZ  --> 1us计数21次   1000000us计数21000000
	while ((SysTick->CTRL & 0x00010000)==0);//等待计数完成,则表示时间到达
	SysTick->CTRL = 0; 			// 关闭定时器
}

//延时毫秒  参数不能超过798ms
void delay_ms(uint32_t nms)
{
	SysTick->CTRL = 0; 					// 关闭定时器
	SysTick->LOAD = nms*21*1000 - 1; 	// 指的是延时时间对应的计数次数 			
	SysTick->VAL = 0; 					// 清空当前计数值
	SysTick->CTRL = 1; 					// 开启定时器  并且时钟频率是21MHZ  --> 1us计数21次   1000000us计数21000000
	while ((SysTick->CTRL & 0x00010000)==0);//等待计数完成,则表示时间到达
	SysTick->CTRL = 0; 					// 关闭定时器
}

//延时秒
void delay_s(uint32_t delay)
{
	while(delay--)
	{
		delay_ms(500);
		delay_ms(500);
	}
}




















