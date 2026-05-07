#ifndef __PULSESENSOR_H__
#define __PULSESENSOR_H__
#include "stm32f4xx.h"
#include <stdio.h>
#include "delay.h"

extern uint16_t heart_rate; //ÐÄÂÊ

void PulseSensor_Init(void);
uint16_t ADC_Read(ADC_TypeDef* ADCx,uint8_t ADC_Channel);
void CalculateHeartRate(void);
void delay(void);
void PluseSensor_Sampling(void);
void Timer2_Init(void);
#endif
