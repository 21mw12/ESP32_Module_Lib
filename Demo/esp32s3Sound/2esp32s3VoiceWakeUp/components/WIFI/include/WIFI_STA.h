#ifndef __WIFI_STA_H_
#define __WIFI_STA_H_

#include <stdio.h>

/**
 * WIFI配置信息
 * 名称
 * 密码
 * 从新连接次数
 */
#define WIFI_SSID       "mw-OpenWrt"
#define WIFI_PASSWORLD  "1176224694"
#define MAXIMUM_RETRY   5

/**
 * @brief  WIFI站点模式初始化
 * @retval 无
 */
void WIFI_STA_Init(void);

#endif
