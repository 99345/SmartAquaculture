#include "HAL_uart.h"

uint8_t DebugLevel=LEVEL_ALL;	//打印所有所有小于DebugLevel的信息,DebugLevel可以动态调整

static HAL_UartcallBack_t __uart1CallBackFunc=NULL;
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  

/*这段代码是用来支持printf打印到串口的代码*/
#pragma import(__use_no_semihosting)             
//标准库需要支持的函数
struct __FILE 
{
	int handle;
};

FILE __stdout;       
//定义_sys_exit()以避免工作在半主机状态
void _sys_exit(int x) 
{
	x = x; 
}
//重定义fputc函数
//这个需要根据MCU和我们希望printf从哪个串口输出来确认 __WAIT_TODO__
int fputc(int ch, FILE *f)
{
	//注意：USART_FLAG_TXE是检查发送缓冲区是否为空，这个要在发送前检查，检查这个提议提高发送效率，但是在休眠的时候可能导致最后一个字符丢失
	//USART_FLAG_TC是检查发送完成标志，这个在发送后检查，这个不会出现睡眠丢失字符问题，但是效率低（发送过程中发送缓冲区已经为空了，可以接收下一个数据了，但是因为要等待发送完成，所以效率低）
	//不要两个一起用，一起用效率最低
	
	//循环等待直到发送缓冲区为空(TX Empty)此时可以发送数据到缓冲区
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
  {}
	USART_SendData(USART1, (uint8_t) ch);

  /* 循环等待直到发送结束*/
  //while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  //{}

	return ch;
}

//uart1(PA_9 TX,PA_10 RX)初始化函数
uint8_t HALUart1Init(uint32_t uartBand,HAL_UartcallBack_t uartCallBackFunc){
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	__uart1CallBackFunc=uartCallBackFunc;
	
	USART_DeInit(USART1);  //复位串口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);//开启串口时钟,注意APB1和APB2时钟使能函数不一样
	
	//PA_9 初始化为外设模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz，这里不用传参，直接写死用最大速度50MHZ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//片上外设，推挽模式(需要配合PSEL设置才能确定是复用哪个功能)
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化GPIO
	//PA_10初始化为上拉输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化GPIO

	if(NULL!=uartCallBackFunc){//没有回调函数的时候不启动接收中断，此时的串口能发不能收
		//中断优先级管理，当前中断优先级分组为2(NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2))已经定义，不要修改
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	//指定配置哪个中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级为3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//配置 NVIC
	}

	//USART配置
	USART_InitStructure.USART_BaudRate = uartBand;	//设置串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;	//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无流控
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //配置USART参数
	if(NULL!=uartCallBackFunc){
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//开启接收中断(不开启无法接收数据,没有接收回调函数就不开启了，因为没有回调就没有处理接收的到数据，开启了也没有意义)
	}
	USART_Cmd(USART1, ENABLE);                    //使能USART
	
	return 0;
}

//USART1中断函数(接收)
//这个需要根据MCU适配 __WAIT_TODO__
void USART1_IRQHandler(void){
	if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)  
	{
		if(NULL!=__uart1CallBackFunc){
			(*__uart1CallBackFunc)(USART_ReceiveData(USART1));	//执行回调函数
		}
	}
}

/*//USART2中断函数(接收)
//这个需要根据MCU适配 __WAIT_TODO__
void USART2_IRQHandler(void){
	uint8_t ch;
	if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET)  
	{
		ch=USART_ReceiveData(USART2);	//读取接收到的字符
	}
}

//USART3中断函数(接收)
//这个需要根据MCU适配 __WAIT_TODO__
void USART3_IRQHandler(void){
	uint8_t ch;
	if(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == SET)  
	{
		ch=USART_ReceiveData(USART3);	//读取接收到的字符
	}
}*/
