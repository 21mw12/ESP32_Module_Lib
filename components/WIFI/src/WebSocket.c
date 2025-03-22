#include "WebSocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_websocket_client.h"

// 日志标签
static const char *TAG = "WebSocket_Client";

esp_websocket_client_handle_t websocket_client_handle;

// WebSocket事件处理函数
static void _websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    // 将事件数据转换为esp_websocket_event_data_t类型
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    // 根据事件ID进行不同的处理
    switch (event_id) {
        case WEBSOCKET_EVENT_CONNECTED:
            // WebSocket连接成功
            ESP_LOGI(TAG, "WebSocket connected");
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            // WebSocket连接断开
            ESP_LOGI(TAG, "WebSocket disconnected");
            break;
        case WEBSOCKET_EVENT_DATA:
            // 接收到WebSocket数据
            ESP_LOGI(TAG, "Received data: %.*s", data->data_len, (char *)data->data_ptr);
            break;
        case WEBSOCKET_EVENT_ERROR:
            // WebSocket发生错误
            ESP_LOGE(TAG, "WebSocket error");
            break;
        default:
            // 其他事件
            break;
    }
}

void WebSocket_Init(void) {
    // 定义WebSocket配置
    const esp_websocket_client_config_t ws_cfg = {
        .uri = WEBSOCKET_URI, // WebSocket URI
    };
    // 初始化WebSocket客户端
    websocket_client_handle = esp_websocket_client_init(&ws_cfg);
    // 注册WebSocket事件处理函数
    esp_websocket_register_events(websocket_client_handle, WEBSOCKET_EVENT_ANY, _websocket_event_handler, websocket_client_handle);
    // 启动WebSocket客户端
    esp_websocket_client_start(websocket_client_handle);
}

void WebSocket_Sent(int16_t *data_byte_stream) {
    // 发送二进制数据
    int sent = esp_websocket_client_send_bin(websocket_client_handle, (const char *)data_byte_stream, BUFFER_SIZE, portMAX_DELAY);
    // 如果发送成功
    if (sent > 0) {
        // 打印发送的字节数
        ESP_LOGI(TAG, "Sent %d bytes of binary data", sent);
    } else {
        // 打印发送失败
        ESP_LOGE(TAG, "Failed to send binary data");
    }
}
