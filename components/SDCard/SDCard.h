#ifndef __SD_CARD_H_
#define __SD_CARD_H_

#include <stdio.h>
#include "esp_err.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_camera.h"

#define PIN_NUM_MISO  GPIO_NUM_2
#define PIN_NUM_MOSI  GPIO_NUM_15
#define PIN_NUM_CLK   GPIO_NUM_14
#define PIN_NUM_CS    GPIO_NUM_13

#define EXAMPLE_MAX_CHAR_SIZE    64

#define MOUNT_POINT "/sdcard"

/**
 * @brief  SD卡初始化
 * @retval 无
 */
esp_err_t SDCard_Init(void);

/**
 * @brief  SD卡卸载
 * @retval 无
 */
void SDCard_DeInit(void);

/**
 * @brief  SD卡格式化
 * @retval 无
 */
esp_err_t SDCard_Format(void);

/**
 * @brief  SD卡文件写入
 * @param filename 文件名
 * @param data 写入的数据
 * @retval 无
 */
esp_err_t SDCard_WriteFile(const char *filename, const char *data);

/**
 * @brief  SD卡文件读取（仅读取显示第一行）
 * @param filename 文件名
 * @retval 无
 */
esp_err_t SDCard_ReadFile(const char *filename);

/**
 * @brief  SD卡文件保存图片
 * @param filename 文件名
 * @retval 无
 */
esp_err_t SDCard_SaveImage(const char* filename);

#endif
