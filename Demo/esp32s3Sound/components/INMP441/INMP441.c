#include "INMP441.h"
#include "esp_err.h"
#include <driver/i2s.h>

int16_t INMP441_sBuffer[INMP441_BufferLen];

void INMP441_Init(){
    // 配置I2S
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX), // 设置为主接收模式
        .sample_rate = 16000, // 设置采样率为16000Hz
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // 设置采样位数为16位
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, // 设置通道格式为单声道
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S), // 设置通信格式为标准I2S
        .intr_alloc_flags = 0, // 分配中断标志
        .dma_buf_count = 8, // 设置DMA缓冲区数量为8
        .dma_buf_len = INMP441_BufferLen, // 设置DMA缓冲区长度
        .use_apll = false          // 分配中断标志
    };
    // 安装I2S驱动
    i2s_driver_install(INMP441_PORT, &i2s_config, 0, NULL);

    // 配置I2S引脚
    i2s_pin_config_t pin_config = {
        .bck_io_num = INMP441_SCK, // 设置BCK引脚
        .ws_io_num = INMP441_WS, // 设置WS引脚
        .data_out_num = -1, // 设置数据输出引脚
        .data_in_num = INMP441_SD, // 设置数据输入引脚
    };
    // 设置I2S引脚
    i2s_set_pin(INMP441_PORT, &pin_config);

    // 启动I2S
    i2s_start(INMP441_PORT);
}

int INMP441_Read(void) {
    size_t bytes_read = 0;
    esp_err_t result = i2s_read(INMP441_PORT, INMP441_sBuffer, INMP441_BufferLen * sizeof(int16_t), &bytes_read, portMAX_DELAY);

    if (result != ESP_OK) {
        // 处理错误
        return -1;
    }

    // 返回实际读取的样本数
    return bytes_read / sizeof(int16_t);
}
