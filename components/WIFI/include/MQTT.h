#ifndef __WIFI_MQTT_H_
#define __WIFI_MQTT_H_

#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/**
 * MQTT配置项目
 * 巴法云服务器地址
 * 巴法云密钥
 * 主题名称
 * 向主题发送信息
 */
#define MQTT_URL            "mqtt://bemfa.com:9501/"
#define MQTT_KEY            "5ea8f8735f58d3cf64528cb74ce96774"
#define MQTT_TOPIC          "switch006"
#define MQTT_SENDTOPIC      "switch006/set"

// MQTT消息结构体
typedef struct {
    char* topic;
    char* data;
} MQTTMessage_t;

/**
  * @brief MQTT初始化
  * @param queue MQTT收到消息存放的队列
  * @return 无
  */
void MQTT_Init(QueueHandle_t queue);

/**
  * @brief MQTT获取链接状态
  * @return 链接状态 true表示已连接，false表示未连接
  */
bool MQTT_GetConectedState(void);

/**
  * @brief MQTT发送数据
  * @param data 发送的数据
  * @return 无
  */
void MQTT_SendData(char* data);

#endif
