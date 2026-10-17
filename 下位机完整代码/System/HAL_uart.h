#ifndef __HAL_UART_H__
#define __HAL_UART_H__

#include "stm32f10x.h"
#include "stdio.h"

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收

//打印所有所有小于DebugLevel的信息
#define LEVEL_OFF			0			//关闭打印
#define LEVEL_ERR			1			//错误信息
#define LEVEL_WARNING	2			//警告信息
#define LEVEL_INFO		3			//普通消息
#define LEVEL_DEBUG		4		//调试信息
#define LEVEL_ALL			255		//开启所有打印
extern uint8_t DebugLevel;
#if HAVE_OS
	#define myPrintf(Level, ...) \
	do{                                   \
		vPortEnterCritical(); \
		if ( Level<DebugLevel ){      \
			if(LEVEL_DEBUG==Level){	\
				printf("[%s()-%d]",__func__,__LINE__); \
			}	\
			printf(__VA_ARGS__);               \
		}                               \
		vPortExitCritical();			\
	}while(0)
#else
	#define myPrintf(Level, ...) \
	do{                                   \
		if ( Level<DebugLevel ){      \
			if(LEVEL_DEBUG==Level){	\
				printf("[%s()-%d]",__func__,__LINE__); \
			}	\
			printf(__VA_ARGS__);               \
		}                               \
	}while(0)
#endif

typedef void (*HAL_UartcallBack_t)(uint8_t data);

extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	

uint8_t HALUart1Init(uint32_t uartBand,HAL_UartcallBack_t uartCallBackFunc);

#endif	//end of __HAL_UART_H__
