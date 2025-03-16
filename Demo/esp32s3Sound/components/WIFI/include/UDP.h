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

void UDP_Init(void);

int UDP_Send(uint8_t* data, size_t data_len);

#endif