#ifndef __Button_H_
#define __Button_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

/* 按键配置引脚，模式实现上拉 */
#define Button_Pin GPIO_NUM_4

/**
  * @brief 按键初始化
  * @return 无
 */
void Button_Init(QueueHandle_t queue);

#endif
