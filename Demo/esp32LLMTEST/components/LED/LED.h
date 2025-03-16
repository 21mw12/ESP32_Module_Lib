#ifndef __LED_H_
#define __LED_H_

#include <stdio.h>

/* LED配置引脚 */
#define LED_Pin GPIO_NUM_2

/**
  * @brief LED初始化
  * @return 无
 */
void LED_Init(void);

/**
  * @brief LED反转电平
  * @return 无
 */
void LED_Toggle(void);

/**
  * @brief LED设为高电平
  * @return 无
 */
void LED_SetHeight(void);

/**
  * @brief LED设为低电平
  * @return 无
 */
void LED_SetLow(void);

#endif
