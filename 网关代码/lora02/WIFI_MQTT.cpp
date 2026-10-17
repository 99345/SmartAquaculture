/*
 * WIFI+MQTT连接ONENET平台库函数
 * void initWifi()：连接WIFI
 * void initMQTT()：连接MQTT服务器
 * void mqttPublishData(int med,bool doorLocked)：上传数据
 * void MQTT_callback(char* topic, byte* payload, unsigned int length)：回调函数，解析ONENET平台下发的数据
 * int MQTT_Subscribe_Init(void)：MQTT接收初始化，返回1：初始化成功 返回0：初始化失败
 */
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ============== 全局对象 ==============
extern WiFiClient espClient;
extern PubSubClient client;

// ============== WiFi配置 ==============
const char* ssid = "你的wifi名称";
const char* password = "你的wifi密码";

// MQTT配置
const char* mqttHostUrl = "mqtts.heclouds.com";  // MQTT服务器地址
const int mqttPort = 1883;                         // MQTT端口
const char* mqttUser = "1234567890";                         // 产品ID
const char* mqttClientId1 = "test1";     // 设备名称
const char* mqttPassword1 = "version=2018-10-31&res=products%2F1234567890%2Fdevices%2Ftest1&et=1860034873&method=md5&sign=%2BEBBXpBGpUYcne7BtPt1qw%3D%3D";// 设备密钥
const char* mqttClientId2 = "test2";     // 设备名称
const char* mqttPassword2 = "version=2018-10-31&res=products%2F1234567890%2Fdevices%2Ftest2&et=1860034873&method=md5&sign=aMN6Iq8D4hC5AfmjK7vlSQ%3D%3D";// 设备密钥


//MQTT组包
#define ONENET_FORMAT "{\"id\":\"123\",\"params\":{\"tem\":{\"value\":%.1f},\"level\":{\"value\":%.1f},\"tds\":{\"value\":%.1f},\"dirty\":{\"value\":%.1f},\"ph\":{\"value\":%.1f}}}" //根据mqtt协议拼接字符串，变量必须是onenet设备中已有的变量
#define ONENET_POST "$sys/1234567890/test%d/thing/property/post"  //将产品id拼接到第1与2个下划线的中间

  extern float tem;
  extern float level;
  extern float tds;
  extern float dirty;
  extern float ph;



// ============== 函数声明 ==============
void initWifi();
void initMQTT();

// ============== WiFi初始化 ==============
void initWifi() {
  WiFi.begin(ssid, password);
  int attempts = 0;
  Serial.print("Connecting to WIFI");
  
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi Failed");
  }
}


// ============== MQTT初始化 ==============
void initMQTT() {
  client.setServer(mqttHostUrl, mqttPort);
  while (!client.connected()){
    Serial.println("connecting to MQTT...");
    if (client.connect(mqttClientId1,mqttUser,mqttPassword1)) {
      Serial.println("MQTT test1 connected");
    } 
    else {
      Serial.print("MQTT test1 connect failed,state");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

// ============== MQTT上传信息 ==============
void mqttPublishData(int num,float tem,float level,float tds,float dirty,float ph) {
  static int number[2] = {1};   //存储上一次发送消息时的机器编号,初始化为1
  char param[256];
  char jsonBuf[512];

  number[1] = num;

  sprintf(param,ONENET_POST,num);

  if (number[0] == 1 && number[1] == 2){
    client.disconnect();
    if (client.connect(mqttClientId2,mqttUser,mqttPassword2)) {
      Serial.println("MQTT test2 connected");
    } 
    else {
      Serial.print("MQTT test2 connect failed,state");
      Serial.print(client.state());
      delay(2000);
    }
  }
  else if (number[0] == 2 && number[1] == 1){
    client.disconnect();
    if (client.connect(mqttClientId1,mqttUser,mqttPassword1)) {
      Serial.println("MQTT test1 connected");
    } 
    else {
      Serial.print("MQTT test1 connect failed,state");
      Serial.print(client.state());
      delay(2000);
    }
  }

  number[0] = number[1];

  sprintf(jsonBuf,ONENET_FORMAT,tem,level,tds,dirty,ph);
  Serial.print(num);
  Serial.println(jsonBuf);
  client.publish(param,jsonBuf);
}
