#include "stm32f10x.h"                  // Device header
#include "slave.h"
#include "Delay.h"
#include "AD.h"
#include "ds18b20.h"
#include "sys.h"
#include "usart.h"
#include "math.h"
#include "LED.h"

#define WARNING_PERCENT	70				//水位警告阈值，超过阈值启动水泵


uint16_t AD[4];	//定义AD值变量
float voltage[4];
short short_tem;
float Send_sensor[5];			//待发送变量

Sensor slave_control(void){
		Sensor sensor;
		AD[0] = AD_GetValue(ADC_Channel_0);		//单次启动ADC，转换通道0
		AD[1] = AD_GetValue(ADC_Channel_1);		//单次启动ADC，转换通道1
		AD[2] = AD_GetValue(ADC_Channel_2);		//单次启动ADC，转换通道2
		AD[3] = AD_GetValue(ADC_Channel_3);		//单次启动ADC，转换通道3
		
		for (int i = 0;i < 4;i++){
		voltage[i] = (float)AD[i] / 4095 * 3.3f;
		}
		
		sensor.level = (voltage[0]-0.1)*270;
		if(sensor.level <= 0) sensor.level = 0.0;
		else if(sensor.level >= 100) sensor.level = 100.0;
	    sensor.level = roundf(sensor.level * 10) / 10.0f;     //保留一位小数
	  
		sensor.tds = 360*voltage[1];
	  	sensor.tds = roundf(sensor.tds * 10) / 10.0f;

		sensor.dirty = -2000*voltage[2] +3830;
		if(sensor.dirty <= 0) sensor.dirty = 0.0;
	    sensor.dirty = roundf(sensor.dirty * 10) / 10.0f;

		sensor.ph = -5.634 * voltage[3] + 16.413;
	  	sensor.ph = roundf(sensor.ph * 10) / 10.0f;
		
		short_tem = DS18B20_Get_Temp();
		sensor.tem = (float)short_tem / 10.0f;  // 转换为摄氏度
		
		//水泵控制
		if(sensor.level > WARNING_PERCENT) {
			motor_ON();
		}
		else{
			motor_OFF();
		}
		
		//蜂鸣器控制
		if(sensor.tem >30 || sensor.tem < 10 || sensor.tds >1000 || sensor.dirty > 1500 || sensor.ph >7 || sensor.ph <4){
		bee_ON();
		}
		else{
		bee_OFF();
		}
		
//	  	printf("\r\ntem1 :%.1f C , level2 :%.1f %% , tds3 :%.1f ppm , dirty4 :%.1f ,ph5 :%.1f\r\n",sensor.tem,sensor.level,sensor.tds,sensor.dirty,sensor.ph);
		
		return sensor;

}
