#include "WakeUp.h"
#include "INMP441.h"

#include "esp_mn_models.h"
#include "model_path.h"
#include "esp_mn_iface.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "esp_afe_sr_models.h"
#include "esp_afe_sr_iface.h"

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
    // int afe_chunksize = afe_handle->get_fetch_chunksize(afe_data);
    printf("------------detect start------------\n");

    while (1) {
        afe_fetch_result_t* res = afe_handle->fetch(afe_data); 
        if (!res || res->ret_value == ESP_FAIL) {
            printf("fetch error!\n");
            break;
        }

        // 得到唤醒词
        if (res->wakeup_state == WAKENET_DETECTED) {
	        printf("model index:%d, word index:%d\n", res->wakenet_model_index, res->wake_word_index);
            printf("-----------LISTENING-----------\n");
        }
        
    }
    vTaskDelete(NULL);
}

void WakeUp_Init(void) {
    srmodel_list_t *models = esp_srmodel_init("model");
    afe_config_t *afe_config = afe_config_init("M", models, AFE_TYPE_SR, AFE_MODE_HIGH_PERF);

    // 获取句柄
    afe_handle = esp_afe_handle_from_config(afe_config);
    // 创建实例
    esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(afe_config);

    xTaskCreatePinnedToCore(&SR_feed_Task, "feed", 8 * 1024, (void*)afe_data, 5, NULL, 0);
    xTaskCreatePinnedToCore(&SR_detect_Task, "detect", 4 * 1024, (void*)afe_data, 5, NULL, 1);
}