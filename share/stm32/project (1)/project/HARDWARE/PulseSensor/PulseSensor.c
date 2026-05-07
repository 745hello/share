/************************************************
 神舟号 STM32F407开发板 
 安徽中科康芯科技有限公司  
************************************************/
#include "PulseSensor.h"
// 硬件配置
#define ADC_CHANNEL      ADC_Channel_11  // PC1对应ADC_NUM通道11
#define ADC_NUM 		 ADC1

// 全局变量 & 宏
#define SAMPLE_RATE      200             // 采样率200Hz（最好是1000的因数）
#define BUFFER_SIZE      500             // 采样缓冲区大小
uint16_t adc_buffer[BUFFER_SIZE];
uint32_t sample_index = 0; //用于遍历上面数组
uint16_t heart_rate = 0; //心率


/*******心率传感器初始化 *********/
/***  单片机---心率传感器	*****/
/***  PC1  ---> S引脚		*****/	
/***  5V  ---> 	 +			*****/
/***  GND  --->	 -			*****/	
/********************************/
void PulseSensor_Init(void)
{
	//Timer2_Init();
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	//1.打开ADC_NUM和GPIOC的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	//2.配置PC1引脚  模拟模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//3.配置ADC_NUM的参数 
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;					//独立模式	
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;					//预分频值  21MHZ
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; 	//不使用DMA
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//采样间隔
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	//4.配置ADC_NUM的通道
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;			//转换精度
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;								//不需要扫描
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;					//不需要连续转换
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //不需要
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;						//数据对齐,右对齐
	ADC_InitStructure.ADC_NbrOfConversion = 1;									//通道数量
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO; 		// TIM2触发
	ADC_Init(ADC_NUM, &ADC_InitStructure);

	
	//4.选择ADC_NUM的规则通道(11),设置转换顺序 
	ADC_RegularChannelConfig(ADC_NUM, ADC_CHANNEL, 1, ADC_SampleTime_480Cycles);

	//6.使能ADC_NUM
	ADC_Cmd(ADC_NUM, ENABLE);
	for(uint32_t i=1; i<BUFFER_SIZE-1; i++)
	{
		adc_buffer[i] = 0;
	}
}



/*
	读ADCx 的 ADC_Channel 通道到的模拟量
*/
uint16_t ADC_Read(ADC_TypeDef* ADCx,uint8_t ADC_Channel) 
{
	ADC_RegularChannelConfig(ADCx, ADC_Channel, 1, ADC_SampleTime_480Cycles); // 配置当前要读取的通道
	ADC_SoftwareStartConv(ADCx); // 开始转换
	while(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) == RESET); // 等待转换完成
	return ADC_GetConversionValue(ADCx); // 读取转换结果
	//获取的是数字量  0~4095 编码 十进制
}


void CalculateHeartRate(void)
{
 
	uint16_t first_max = 0; //到达波底之前的最大值
	uint16_t second_max= 0; //波底之后的最大值
	
    uint16_t first_location = 0; //到达波底之前的最大值的位置
    uint16_t second_location = 0; //波底之后的最大值的位置
	
	uint16_t bottom = adc_buffer[0]; //波底的值
//	uint16_t bottom_location = 0; //波底位置

	uint32_t i;
	// 1. 寻找波低的值
    for(i=0; i<BUFFER_SIZE; i++) 
	{
        if(adc_buffer[i] <= bottom) 
		{
			bottom = adc_buffer[i];
			//bottom_location = i;
		}
    }
	
    for(i=0; i<BUFFER_SIZE; i++) 
	{
		// 2. 到达波底之前的最大值 和位置
        if(adc_buffer[i] > first_max) 
		{
			first_max = adc_buffer[i];
			first_location = i;
		}
		if(adc_buffer[i] == bottom) //到达波低
		{
			break;
		}
    }
	for(;i<BUFFER_SIZE;i++) //继续寻找波低之后的最大值
	{
		// 3. 寻找到达波底之后的最大值和位置
        if(adc_buffer[i] > second_max) 
		{
			second_max = adc_buffer[i];
			second_location = i;
		}
    }
	//printf("%d---%d---%d--%d--%d--%d\n",first_max,first_location,bottom,bottom_location,second_max,second_location);
	// 4. 计算心率
	heart_rate = (uint16_t)(60.0f * SAMPLE_RATE / (second_location - first_location));
	if(heart_rate<=50 || heart_rate >= 130)
	{
		heart_rate = 0;
	}
	//printf("hr = %d\n",heart_rate);
	return;
}

/*采样时间间隔*/
void delay(void)
{
	int i = 1000/SAMPLE_RATE;
	for(int j = 1;j<(i*2);j++)
	{
		delay_us(500);
	}
	delay_us(400);
}
/*数据采集，一次采集500份*/
void PluseSensor_Sampling(void)
{
	uint16_t sample;
	loop: sample= ADC_Read(ADC_NUM,ADC_CHANNEL);
	if(sample == 4095 || sample <2000)
	{
		goto loop;
	}
	adc_buffer[sample_index++] = sample;
	if(sample_index >= BUFFER_SIZE)
	{
		//printf("一轮数据采集完成\n");
		sample_index = 0;
		CalculateHeartRate();
		for(uint32_t i=1; i<BUFFER_SIZE-1; i++)
		{
			adc_buffer[i] = 0;
		}
	}
	delay();
}










