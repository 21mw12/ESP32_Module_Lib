#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"
#include "WIFI_STA.h"
#include "HTTP.h"

static const char *TAG = "APP";

void app_main(void) {
    // 初始化NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 启动WiFi
    WIFI_STA_Init();

    // 发送POST请求示例
    cJSON *json_root = cJSON_CreateObject();
    cJSON_AddStringToObject(json_root, "model", "qwen2.5:32b");
    cJSON_AddStringToObject(json_root, "prompt", "嗨，请介绍下你自己");
    cJSON_AddBoolToObject(json_root, "stream", false);
    char *post_data = cJSON_PrintUnformatted(json_root);
    cJSON* json_response = HTTP_Post_Request("http://192.168.180.190:11434/api/generate", post_data);
    if (json_response == NULL) {
        ESP_LOGE(TAG, "Failed to parse JSON response");
    } else {
        // 提取并显示字段内容
        cJSON *response_field = cJSON_GetObjectItem(json_response, "response");
        if (response_field != NULL) {
            ESP_LOGI(TAG, "Response content: %s", response_field->valuestring);
        } else {
            ESP_LOGE(TAG, "Field 'response' not found in JSON");
        }
        cJSON_Delete(json_response);
    }
    cJSON_Delete(json_root);
    free(post_data);
}