#include "stm32f10x.h"                  // Device header
#include "usart.h"


/**
  * 函    数：GPIOA初始化
  * 参    数：无
  * 返 回 值：无
  */
void GPIOA_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;	//开启蜂鸣器(PA12)与水泵(PA11)串口
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);						//将PA1和PA2引脚初始化为推挽输出
	
	/*设置GPIO初始化后的默认电平*/
	GPIO_ResetBits(GPIOA, GPIO_Pin_11);							//水泵初始低电平
	GPIO_SetBits(GPIOA, GPIO_Pin_12);							//蜂鸣器初始高电平
}

/**
  * 函    数：水泵开启
  * 参    数：无
  * 返 回 值：无
  */
void motor_ON(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_11);
}

/**
  * 函    数：水泵关闭
  * 参    数：无
  * 返 回 值：无
  */
void motor_OFF(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_11);
}

/**
  * 函    数：水泵状态判断（1：开启 0：关闭）
  * 参    数：水泵状态
  * 返 回 值：无
  */
void LED1_Turn(int motor)
{
	if (motor == 0)		//如果传入参数为0
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_11);					//则设置PA11引脚为低电平
	}
	else if (motor == 1)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_11);					//则设置PA11引脚为高电平
	}
	else
	{
	printf("int motor error!");								//传入参数错误
	}
}

/**
  * 函    数：蜂鸣器开启
  * 参    数：无
  * 返 回 值：无
  */
void bee_ON(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_12);		//设置PA12引脚为低电平
}

/**
  * 函    数：蜂鸣器关闭
  * 参    数：无
  * 返 回 值：无
  */
void bee_OFF(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_12);		//设置PA12引脚为高电平
}

/**
  * 函    数：蜂鸣器状态翻转（1：开启 0：关闭）
  * 参    数：蜂鸣器状态
  * 返 回 值：无
  */
void bee_Turn(int bee)
{
	if (bee == 0)		//
	{                                                  
		GPIO_SetBits(GPIOA, GPIO_Pin_12);               		//则设置PA12引脚为高电平
	}                                                  
	else if (bee == 1)
	{                                                  
		GPIO_ResetBits(GPIOA, GPIO_Pin_12);             		//则设置PA12引脚为低电平
	}
	else
	{
		printf("int bee error!");
	}
}
