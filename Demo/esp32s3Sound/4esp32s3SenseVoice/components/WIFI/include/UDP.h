#ifndef __WIFI_UDP_H_
#define __WIFI_UDP_H_

#include <stdio.h>

/**
 * WIFI配置信息
 * 名称
 * 密码
 */
#define UDP_HostIPAddress   "192.168.31.171"
#define UDP_HostPort        8888

/**
  * @brief  初始化UDP
  * @retval 无
  */
void UDP_Init(void);

/**
  * @brief  发送UDP数据
  * @param  data 数据指针
  * @param  data_len 数据长度
  * @retval 发送数据长度
  */
int UDP_Send(uint8_t* data, size_t data_len);

/**
  * @brief  接收UDP数据
  * @param  data 数据指针
  * @param  data_len 数据长度
  * @param  timeout_ms 超时时间
  * @retval 接收数据长度
  */
int UDP_Receive(uint8_t* data, size_t data_len, int timeout_ms);

/**
  * @brief  启动UDP服务器
  * @param  handler 回调函数，用于处理接收到的数据
  * @retval 无
  */
void UDP_Start_Server(void (*handler)(const uint8_t *data, int len));

#endif