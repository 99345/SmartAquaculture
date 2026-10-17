#ifndef WIFI_MQTT_h
#define WIFI_MQTT_h

#include "Arduino.h"

// 声明函数
void initWifi(void);
void initMQTT(void);
void mqttPublishData(int num,float tem,float level,float tds,float dirty,float ph);

#endif