#ifndef __AS6221_H__
#define __AS6221_H__
#include "stm32f4xx.h"
#include "delay.h"

// 软件IIC引脚定义
#define IIC_SCL_PORT    GPIOB
#define IIC_SDA_PORT    GPIOB
#define IIC_SCL_PIN     GPIO_Pin_8
#define IIC_SDA_PIN     GPIO_Pin_9
#define SDA(x) x? GPIO_SetBits(IIC_SDA_PORT, IIC_SDA_PIN):GPIO_ResetBits(IIC_SDA_PORT, IIC_SDA_PIN)
#define SCL(x) x? GPIO_SetBits(IIC_SCL_PORT, IIC_SCL_PIN):GPIO_ResetBits(IIC_SCL_PORT, IIC_SCL_PIN)
uint8_t READ_SDA(void);
void SDA_SET_IN(void);
void SDA_SET_OUT(void);
void AS6221_IIC_Init(void);

void AS6221_I2C_Start(void);
void AS6221_I2C_Stop(void);
uint8_t I2C_RecvACK(void);
uint8_t I2C_RecvByte(void);
void I2C_SendByte(uint8_t  dat);
void I2C_SendACK(uint8_t ack);
void I2C_ReadCommand(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *rev_data, uint16_t length);
void I2C_WriteCommand(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *send_data, uint16_t length);


#define AS6221_I2CADDR (0x48)

void AS6221_ReadCommand(uint8_t reg_addr, uint8_t *rev_data, uint8_t length);
void AS6221_WriteCommand(uint8_t reg_addr, uint8_t *send_data, uint16_t len);
float AS6221_GetTemperature(void);
float AS6221_GetTLOW(void);
void AS6221_SetTLOW(int16_t TLow);
float AS6221_GetTHIGH(void);
void AS6221_SetTHIGH(int16_t THigh);
uint16_t AS6221_GetConfig(void);
void AS6221_SetConfig(uint16_t config);

#endif


