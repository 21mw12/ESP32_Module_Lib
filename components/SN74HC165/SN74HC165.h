#ifndef __74HC165_H
#define __74HC165_H

#include <stdint.h>

// 定义串联的74HC165芯片数量
#define NUM_OF_74HC165  2

// 定义引脚
#define PIN_SER_OUT     GPIO_NUM_25
#define PIN_CLK         GPIO_NUM_26
#define PIN_SH_LD       GPIO_NUM_27

/* 数据数组，一个下标表示一个芯片的数据 */
extern uint8_t SN74HC165Data[NUM_OF_74HC165];

/**
  * @brief 初始化74HC165
  * @return 无
 */
void SN74HC165_Init(void);

/**
  * @brief 读取74HC165数据
  * @return 无
 */
void SN74hc165_read(void);

#endif