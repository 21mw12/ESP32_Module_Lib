#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include <nvs_flash.h>

#include "INMP441.h"
#include "WIFI_STA.h"
#include "UDP.h"

void app_main(void) {
    // 初始化 NVS flash
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 初始化 INMP441
    INMP441_Init();
    // 初始化 WIFI STA
    WIFI_STA_Init();
    // 初始化 UDP
    UDP_Init();

    while(1) {
        // 读取 INMP441 数据
        INMP441_Read();
        // 发送 UDP 数据
        UDP_Send((uint8_t*)INMP441_sBuffer, INMP441_BufferLen * sizeof(int16_t));

        // 每循环一次释放 CPU
        vTaskDelay(1);
    }
}