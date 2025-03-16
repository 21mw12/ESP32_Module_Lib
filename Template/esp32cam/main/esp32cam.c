#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>
#include <esp_wifi.h>
#include <esp_event.h>

#include "LED.h"
#include "WIFI_STA.h"
#include "Camera.h"
#include "HTTP.h"
#include "SDCard.h"

static const char *TAG = "APP";

void app_main(void) {
    // 初始化NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 补光灯默认关闭
    LED_Init();
    LED_SetLow();

    // 初始化SD卡
    SDCard_Init();

    // 初始化WIFI
    WIFI_STA_Init();

    // 初始化摄像头
    if (Camera_Init() != ESP_OK) {
        return;
    }

    // 拍照并保存到SD卡
    SDCard_SaveImage("/sdcard/camera.jpg");

    // 启动HTTP服务器
    HTTP_Start_Webserver("/esp32", Camera_jpg_Stream_Handler);
}
