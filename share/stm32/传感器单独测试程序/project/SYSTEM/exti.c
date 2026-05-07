#include "exti.h"
uint16_t Key_Times = 0; //按键次数
/*按键0-->外部中断4初始化*/
void Exti4_Init(void)
{
	//1. 初始化GPIO
	GPIO_InitTypeDef GPIO_initstruct; //定义结构体信息
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE); //使能GPIOE的时钟
	GPIO_initstruct.GPIO_Pin = GPIO_Pin_4; //指定4号引脚
	GPIO_initstruct.GPIO_Mode  = GPIO_Mode_IN; //指定输入模式
	GPIO_initstruct.GPIO_OType =  GPIO_OType_PP;
	GPIO_initstruct.GPIO_Speed = GPIO_Speed_2MHz; //低速
	GPIO_initstruct.GPIO_PuPd = GPIO_PuPd_UP; //上拉	
	GPIO_Init(GPIOE,&GPIO_initstruct); //初始化结构体
	
	//2. 配置SYSCFG选择器
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource4);
	
	//3. 初始化外部中断控制器
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line		=	EXTI_Line4;
	EXTI_InitStruct.EXTI_Mode		=	EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger	=	EXTI_Trigger_Falling;
	EXTI_InitStruct.EXTI_LineCmd	=	ENABLE;				
	EXTI_Init(&EXTI_InitStruct);
	
	//4. 初始化NVIC中断控制器
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel						=	EXTI4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	=	2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority			=	2;
	NVIC_InitStruct.NVIC_IRQChannelCmd					=	ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void EXTI4_IRQHandler(void)
{
	Key_Times++;
	if(Key_Times == 10)
	{
		Key_Times = 0;
	}
	EXTI_ClearITPendingBit(EXTI_Line4);

}


