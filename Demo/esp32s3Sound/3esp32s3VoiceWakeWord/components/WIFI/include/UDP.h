#ifndef __WIFI_UDP_H_
#define __WIFI_UDP_H_

#include <stdio.h>

/**
 * WIFI配置信息
 * 名称
 * 密码
 * 从新连接次数
 */
#define UDP_HostIPAddress   "192.168.31.171"
#define UDP_HostPort        8888

/**
 * @brief  UDP初始化
 * @retval 无
 */
void UDP_Init(void);

/**
 * @brief  UDP发送数据
 * @param  data     数据
 * @param  data_len 数据长度
 * @retval 无
 */
int UDP_Send(uint8_t* data, size_t data_len);

#endif