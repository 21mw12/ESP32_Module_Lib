#ifndef __INMP441_H_
#define __INMP441_H_

///////////////////////////////////////////////////////////
//
// 文件功能：INMP441的基础功能
// 版本：V1.0
// 作者：墨蔚（MW）
// 修改时间：2025/03/16
// 项目来源：https://www.bilibili.com/video/BV1g49tYqEVe?spm_id_from=333.788.videopod.sections&vd_source=8566cec36593b0e28ee03f3c724b87d0
//
// 模块信息：
//      1. 工作电压：1.8V - 3.3V
//      2. 温度范围：-40°C - +85°C
//      3. 信噪比：61 dB
//      4. 频响范围：60 Hz - 15 kHz
//
// 引脚信息：
//      1. VDD：电源正极
//      2. GND：电源负极
//      3. SD：串行数据输出引脚，通过I2S接口传输数字音频数据。
//      4. SCK：串行时钟输入引脚，用于I2S接口的时钟信号。
//      5. WS：字选择输入引脚，用于I2S接口的帧同步信号（也称为LRCK）。
//      6. L/R：左右声道选择引脚。
//          当该引脚设置为低电平时，麦克风在I²S帧中输出其信号到左声道。
//          当该引脚设置为高电平时，麦克风在I²S帧中输出其信号到右声道。
//
///////////////////////////////////////////////////////////

#include <stdio.h>

/* INMP441引脚定义 */
#define INMP441_WS      16
#define INMP441_SD      17
#define INMP441_SCK     15
#define INMP441_PORT    I2S_NUM_0

/* 缓冲区大小 */
#define INMP441_BufferLen   512

/* 采集到的数据 */
extern int16_t INMP441_sBuffer[INMP441_BufferLen];

/**
  * @brief INMP441初始化
  * @return 无
 */
void INMP441_Init(void);

/**
  * @brief INMP441读取数据
  * @return 实际读取的样本数
 */
int INMP441_Read(void);

#endif
