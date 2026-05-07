/************************************************
 神舟号 STM32F407开发板 
 安徽中科康芯科技有限公司  
************************************************/
#include "AS6221.h"
/*
	传感器	单片机
	VCC -- 		VCC
	GND		--	GND
	SDA 	--	PB9
	SCL		--	PB8
*/
//AS6221初始化--> 体温
void AS6221_IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
		
	//1.使能GPIO外设时钟  GPIOB
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	//2.配置GPIO引脚的参数  输出模式
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin   = IIC_SDA_PIN | IIC_SCL_PIN;
	GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);
	//3.SDA引脚和SCL引脚处于空闲状态
	SDA(1);
	SCL(1);
}
//获取温度
float AS6221_GetTemperature(void)
{
    uint8_t char_temp[2];
    int16_t int_temp = 0;
    float float_temp = 0.0;

    AS6221_ReadCommand(0x00, char_temp, 2);

    int_temp = char_temp[0] << 8 | char_temp[1];
    if (int_temp < 32767)
        float_temp = (float)int_temp * (float)0.0078125;

    if (int_temp >= 32767)
        float_temp = (float)((int_temp - 1) * 0.0078125) * -1;
    return float_temp;
}
//设置SDA引脚为输入模式
void SDA_SET_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	//1.使能GPIO外设时钟  GPIOB
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//2.配置GPIO引脚的参数  输入模式
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin   = IIC_SDA_PIN;			//SDA
	GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);
}

//设置SDA引脚为输出模式
void SDA_SET_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
		
	//1.使能GPIO外设时钟  GPIOB
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//2.配置GPIO引脚的参数  输出模式
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin   = IIC_SDA_PIN;		
	GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);
}

uint8_t READ_SDA(void)
{
	return GPIO_ReadInputDataBit(IIC_SDA_PORT, IIC_SDA_PIN);
}

//I2C起始信号
void AS6221_I2C_Start(void)
{
	SDA_SET_OUT();
	SDA(1);	//拉高数据线
	SCL(1);	//拉高时钟线
	delay_us(5); //延时
	SDA(0);	//产生下降沿
	delay_us(5); //延时
	SCL(0);	//拉低时钟线
}

//I2C停止信号
void AS6221_I2C_Stop(void)
{
	SDA_SET_OUT();
	SDA(0);	//拉低数据线
	SCL(1);	//拉高时钟线
	delay_us(5); //延时
	SDA(1);	//产生上升沿
	delay_us(5); //延时
}


/*
I2C发送应答信号
入口参数:ack (0:ACK 1:NAK)
*/
void I2C_SendACK(unsigned char ack)
{
	SDA_SET_OUT();
	SCL(0);
	SDA(ack);	//写应答信号
	delay_us(5); //延时
	SCL(1);	//拉高时钟线
	delay_us(5); //延时
	SCL(0);	//拉低时钟线
}


//I2C接收应答信号
uint8_t I2C_RecvACK(void)
{
	SDA_SET_IN();
	uint8_t wait = 0xff;
	SCL(1);
	while (READ_SDA() && wait--);
	if (wait <= 0)
	{
		AS6221_I2C_Stop();
		return 255;
	}
	SCL(0);
	delay_us(2); //延时
	return 0;
}


//向I2C总线发送一个字节数据
void I2C_SendByte(uint8_t dat)
{
	uint8_t i;
	SDA_SET_OUT();
	for (i = 0; i < 8; i++) //8位计数器
	{
		if (dat & 0x80)
			SDA(1);
		else
			SDA(0);
		SCL(1);	//拉高时钟线
		delay_us(5); //延时
		SCL(0);	//拉低时钟线
		delay_us(5); //延时
		dat <<= 1;
	}
	//I2C_RecvACK();
}


//从I2C总线接收一个字节数据
uint8_t I2C_RecvByte()
{
	SDA_SET_IN();
	uint8_t i;
	uint8_t dat = 0;
	SDA(1);				//使能内部上拉,准备读取数据,
	for (i = 0; i < 8; i++) //8位计数器
	{
		dat <<= 1;
		SCL(1);	//拉高时钟线
		delay_us(2); //延时
		dat |= READ_SDA(); //读数据
		SCL(0);	//拉低时钟线
		delay_us(2); //延时
	}
	return dat;
}


//从I2C总线接收字符串
void I2C_ReadCommand(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *rev_data, uint16_t length)
{
	SDA_SET_IN();
	AS6221_I2C_Start();
	I2C_SendByte((i2c_addr << 1) | 0x00);
	I2C_RecvACK();
	I2C_SendByte(reg_addr);
	I2C_RecvACK();
	AS6221_I2C_Start();
	delay_ms(15);
	I2C_SendByte((i2c_addr << 1) | 0x01);
	I2C_RecvACK();
	while (length)
	{
		*rev_data = I2C_RecvByte();
		if (length == 1)
			I2C_SendACK(1);
		else
			I2C_SendACK(0);
		rev_data++;
		length--;
	}
	AS6221_I2C_Stop();
}


//向I2C总线发送字符串
void I2C_WriteCommand(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *send_data, uint16_t length)
{
	SDA_SET_OUT();
	AS6221_I2C_Start();
	I2C_SendByte((i2c_addr << 1) | 0x00);
	I2C_RecvACK();
	I2C_SendByte(reg_addr);
	I2C_RecvACK();
	while (length)
	{
		I2C_SendByte(*send_data);
		I2C_RecvACK();
		send_data++;
		length--;
	}
	AS6221_I2C_Stop();
}

//AS6221 Read Command
void AS6221_ReadCommand(uint8_t reg_addr, uint8_t *rev_data, uint8_t length)
{
    I2C_ReadCommand(AS6221_I2CADDR, reg_addr, rev_data, length);
    delay_ms(100);
}


//AS6221 Write Command
void AS6221_WriteCommand(uint8_t reg_addr, uint8_t *send_data, uint16_t len)
{
    I2C_WriteCommand(AS6221_I2CADDR, reg_addr, send_data, len);
}

//Get Low temperature threshold value
float AS6221_GetTLOW(void)
{
    uint8_t char_temp[2];
    int16_t int_temp = 0;
    float float_temp = 0.0;

    AS6221_ReadCommand(0x02, char_temp, 2);
    int_temp = char_temp[0] << 8 | char_temp[1];

    if (int_temp < 32767)
        float_temp = (float)int_temp * (float)0.0078125;

    if (int_temp >= 32767)
        float_temp = (float)((int_temp - 1) * 0.0078125) * -1;

    return float_temp;
}


//Set Low temperature threshold value
void AS6221_SetTLOW(int16_t TLow)
{
    uint8_t char_temp[2] = {0};
    char_temp[0] = ((int16_t)(TLow / 0.0078125) >> 8);
    char_temp[1] = (int16_t)(TLow / 0.0078125) & 0xff;
    AS6221_WriteCommand(0x02, char_temp, 2);
}


//Get High temperature threshold value
float AS6221_GetTHIGH(void)
{
    uint8_t char_temp[2];
    int16_t int_temp = 0;
    float float_temp = 0.0;

    AS6221_ReadCommand(0x03, char_temp, 2);
    int_temp = char_temp[0] << 8 | char_temp[1];

    if (int_temp < 32767)
        float_temp = (float)int_temp * (float)0.0078125;

    if (int_temp >= 32767)
        float_temp = (float)((int_temp - 1) * 0.0078125) * -1;

    return float_temp;
}


//Set High temperature threshold value
void AS6221_SetTHIGH(int16_t THigh)
{
    uint8_t char_temp[2] = {0};
    char_temp[0] = ((int16_t)(THigh / 0.0078125) >> 8);
    char_temp[1] = (uint16_t)(THigh / 0.0078125) & 0xff;
    AS6221_WriteCommand(0x03, char_temp, 2);
}


//Get AS6221 Config
uint16_t AS6221_GetConfig(void)
{
    uint8_t char_config[2];
    AS6221_ReadCommand(0x01, char_config, 2);
    return (uint16_t)char_config[0] << 8 | char_config[1];
}

//Set AS6221 Config
void AS6221_SetConfig(uint16_t config)
{
    uint8_t char_config[2];
    char_config[0] = config >> 8;
    char_config[1] = config & 0xff;
    AS6221_WriteCommand(0x01, char_config, 2);
}


