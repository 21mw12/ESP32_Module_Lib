#include "WIFI_STA.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sys.h"

/* 日志所属 */
static const char *TAG = "WIFI_STA";

/**
 * ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD 表示网络认证方式包括：
 * - WIFI_AUTH_OPEN
 * - WIFI_AUTH_WEP
 * - WIFI_AUTH_WPA_PSK
 * - WIFI_AUTH_WPA2_PSK
 * - WIFI_AUTH_WPA_WPA2_PSK
 * - WIFI_AUTH_WPA3_PSK
 * - WIFI_AUTH_WPA2_WPA3_PSK
 * - WIFI_AUTH_WAPI_PSK
 * 
 * ESP_WIFI_SAE_MODE 表示WPA3下的SAE模式
 * - WPA3_SAE_PWE_HUNT_AND_PECK 使用PWE方法，适用于大多数设备，但计算相对较为耗时。
 * - WPA3_SAE_PWE_HASH_TO_ELEMENT 使用哈希到元素方法，更现代化的方法，提供更好的安全性和效率。
 * - WPA3_SAE_PWE_BOTH 同时使用两种
 */
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HUNT_AND_PECK
#ifdef ESP_WIFI_SAE_MODE == WPA3_SAE_PWE_HUNT_AND_PECK
#define EXAMPLE_H2E_IDENTIFIER ""
#else
// 需要额外配置
#define EXAMPLE_H2E_IDENTIFIER CONFIG_ESP_WIFI_PW_ID
#endif

/**
 * WIFI连接的信号组
 * 信号组状态，包括：连接成功 & 连接埠成功
 * WIFI重连次数
 */
static EventGroupHandle_t s_wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
static int s_retry_num = 0;

/*　任何WIFI有关事件的回调函数　*/
static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    // 开始时执行,进行连接
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    }
    // 未连接成功时执行，再次连接
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    }
    // 连接成功，获取IP地址
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void WIFI_STA_Init(void) {
    s_wifi_event_group = xEventGroupCreate();

    /* 初始化WIFI驱动 */
    ESP_ERROR_CHECK(esp_netif_init());  // 初始化TCP/IP网络栈
    ESP_ERROR_CHECK(esp_event_loop_create_default());   // 创建默认的事件循环
    esp_netif_create_default_wifi_sta();    // 创建一个默认的WiFi STA接口
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));   // 使用默认参数初始化WiFi驱动程序

    /* 注册事件处理程序 */
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    // 当任何WiFi事件（如连接、断开连接、扫描完成等）发生时，event_handler会被调用
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    // 当ESP32在STA模式下成功连接到WiFi并获取到IP地址时，event_handler会被调用
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    /* 设置WIFI结构体 */
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORLD,
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
            .sae_h2e_identifier = EXAMPLE_H2E_IDENTIFIER,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA)); // 设置站点模式
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));   // 根据结构体配置
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /**
     * 初始化事件组
     * 连接成功bit0置1，连接失败bit1置1
     * 第一个pdFALSE表示：成功退出时不清除等待的事件位
     * 第二个pdFALSE表示：只要有一个事件位被设置就退出
     * 无限期等待直至WIFI连接成功或失败
     */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                            pdFALSE,
                                            pdFALSE,
                                            portMAX_DELAY);

    /* WIFI连接结果日志 */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", WIFI_SSID, WIFI_PASSWORLD);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", WIFI_SSID, WIFI_PASSWORLD);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}
