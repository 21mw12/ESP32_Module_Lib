#include "INMP441.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include <driver/i2s.h>

#include "esp_mn_models.h"
#include "model_path.h"
#include "esp_mn_iface.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "esp_afe_sr_models.h"
#include "esp_afe_sr_iface.h"

i2s_chan_handle_t handle;
int16_t INMP441_sBuffer[INMP441_BufferLen];

static esp_afe_sr_iface_t *afe_handle = NULL;

void SR_feed_Task(void *arg) {
    esp_afe_sr_data_t *afe_data = arg;
    while (1) {
        INMP441_Read();

        afe_handle->feed(afe_data, INMP441_sBuffer);
    }
    vTaskDelete(NULL);
}

// 将唤醒词检测模块的结果输出到串口
void SR_detect_Task(void *arg) {
    esp_afe_sr_data_t *afe_data = arg;
    int afe_chunksize = afe_handle->get_fetch_chunksize(afe_data);
    printf("------------detect start------------\n");

    while (1) {
        afe_fetch_result_t* res = afe_handle->fetch(afe_data); 
        if (!res || res->ret_value == ESP_FAIL) {
            printf("fetch error!\n");
            break;
        }

        if (res->wakeup_state == WAKENET_DETECTED) {//得到唤醒词
	        printf("model index:%d, word index:%d\n", res->wakenet_model_index, res->wake_word_index);
            printf("-----------LISTENING-----------\n");
        }
        
    }
    vTaskDelete(NULL);
}

void SR_Init(void) {
    srmodel_list_t *models = esp_srmodel_init("model");
    afe_config_t *afe_config = afe_config_init("M", models, AFE_TYPE_SR, AFE_MODE_HIGH_PERF);

    // 获取句柄
    afe_handle = esp_afe_handle_from_config(afe_config);
    // 创建实例
    esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(afe_config);

    xTaskCreatePinnedToCore(&SR_feed_Task, "feed", 8 * 1024, (void*)afe_data, 5, NULL, 0);
    xTaskCreatePinnedToCore(&SR_detect_Task, "detect", 4 * 1024, (void*)afe_data, 5, NULL, 1);
}

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

    SR_Init();
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
