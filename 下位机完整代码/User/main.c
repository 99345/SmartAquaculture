#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "AD.h"
#include "ds18b20.h"
#include "sys.h"
#include "usart.h"
#include "math.h"
#include "LED.h"
#include "slave.h"

#include "stdio.h"
#include "string.h"
#include "stm32f10x_it.h"
#include "radio/sx127x_driver.h"
#include "example/example_TX.h"

#define EQUIPMENT_NUM 2		//定义设备编号

Sensor send_sensor;			//待发送变量


int main(void)
{
	// 系统初始化
    SystemInit();
	//配置中断优先级
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // 设置分组为 4:0 (抢占4位，子优先级0位)

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级设为 0 (最高)
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	/*模块初始化*/

    // 3. 外设初始化
    USART1_Config(); // 你的串口初始化
    // ... 其他初始化

    // 4. 开启全局中断
    __enable_irq();
	
	tLoRaSettings stting={435000000,20,7,12,1,0x0005};	//频率435M，功率20dbm，带宽125kHz，SF=12，误码编码率4/5,前导码长度0x0005
	SysTick_Config(SystemCoreClock/1000);
	printf("version:%s\r\n",VERSION);
	g_Radio.Init(&stting);	//初始化配置
	printf("sx1278 init OK\r\n");
	
	AD_Init();					//AD初始化
	
	GPIOA_Init();
	
	int a = 0;
	a = DS18B20_Init();
	if(a == 1) printf("DS18B20 init error!");
	
	while (1)
	{
		send_sensor  = slave_control();
		exampleTx(EQUIPMENT_NUM,&send_sensor);	//发送测试，发送完成等待1s后再次发送

		delay_ms(1000);			//延时100ms，手动增加一些转换的间隔时间
	}
}
