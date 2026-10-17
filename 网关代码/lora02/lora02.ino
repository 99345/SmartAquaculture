#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "WIFI_MQTT.h"

WiFiClient espClient; 
PubSubClient client(espClient);

//Lora初始化 
#define SS 41
#define RST 1
#define DI0 2
#define BAND 433E6

int num = 0;      //设备编号
float tem = 0, level = 0, tds = 0, dirty = 0, ph = 0;

void setup() {
  Serial.begin(115200);//115200串口
  while (!Serial);
  SPI.begin(38, 39, 40, -1);
  LoRa.setPins(SS, RST, DI0); //设置lora引脚
  Serial.println("LoRa Receiver");
  //--------检查ra-01是否连接成功------
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  //----------------------------------
  // [带宽] 125kHz
  LoRa.setSignalBandwidth(125E3);
  
  // [扩频因子] SF=12
  LoRa.setSpreadingFactor(12);
  
  // [编码率] 4/5 (参数填写分母 5)
  LoRa.setCodingRate4(5);
  
  // [前导码] 0x0005 (默认通常是 8，必须修改以匹配下位机)
  LoRa.setPreambleLength(5);
  //--------这里是通信的信道，------  
  LoRa.setSyncWord(0x12);//接受来自信道0xA5的信号消息
  LoRa.setTxPower(20);

  initWifi();
  initMQTT();
}

void loop() {
 //尝试解析数据包
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
  char buffer[50]; // 定义一个能存50个字符的箱子

  int len = LoRa.readBytes(buffer, sizeof(buffer) - 1); 

  if (len > 0) {
    buffer[len] = '\0'; // 手动添加字符串结束符，防止乱码
    Serial.println(buffer);
  }
    sscanf(buffer,"%d,%f,%f,%f,%f,%f",&num,&tem, &level, &tds, &dirty, &ph);

    Serial.print("Received packet,RSSI:");//信号强度
    Serial.println(LoRa.packetRssi());
    Serial.println("Snr:"); // 信噪比
    Serial.println(LoRa.packetSnr());
    
    LoRa.endPacket();//终止接收包

    mqttPublishData(num,tem, level, tds, dirty, ph);
  }
  client.loop();
}
