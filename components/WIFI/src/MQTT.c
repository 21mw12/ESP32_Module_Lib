#include "MQTT.h"
#include "WIFI_STA.h"
#include "esp_log.h"
#include "mqtt_client.h"

/* 日志所属 */
static const char *TAG = "MQTT";

// MQTT客户端操作句柄
static esp_mqtt_client_handle_t s_mqtt_client = NULL;

// MQTT连接标志
static bool s_is_mqtt_connected = false;

// MQTT消息队列
static QueueHandle_t MQTT_MessageQueue = NULL;

/*　任何MQTT有关事件的回调函数　*/
static void MQTT_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    // 事件处理函数，用于处理MQTT事件
    esp_mqtt_event_handle_t event = event_data;

    // esp_mqtt_client_handle_t client = event->client;
    switch ((esp_mqtt_event_id_t)event_id) {
        /* 连接成功后，订阅测试主题 */
        case MQTT_EVENT_CONNECTED:
            s_is_mqtt_connected = true;
            esp_mqtt_client_subscribe_single(s_mqtt_client, MQTT_TOPIC, 1);
            break;
        /* 连接断开 */        
        case MQTT_EVENT_DISCONNECTED:
            s_is_mqtt_connected = false;
            break;
        /* 收到订阅消息ACK */
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, " mqtt subscribed ack, msg_id=%d", event->msg_id);
            break;
        /* 收到解订阅消息ACK */
        case MQTT_EVENT_UNSUBSCRIBED:
            break;
        /* 收到发布消息ACK */
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "mqtt publish ack, msg_id=%d", event->msg_id);
            break;
        /* 接收到订阅主题发来的消息 */
        case MQTT_EVENT_DATA:
            if (strstr(event->topic, MQTT_TOPIC) != NULL) {
                MQTTMessage_t msg = {
                    .topic = malloc(event->topic_len + 1),
                    .data = malloc(event->data_len + 1)
                };

                if (!msg.topic || !msg.data) {
                    ESP_LOGE(TAG, "Memory allocation failed");
                    free(msg.topic);
                    free(msg.data);
                    break;
                }

                // 复制数据（保证字符串终止）
                memcpy(msg.topic, event->topic, event->topic_len);
                msg.topic[event->topic_len] = '\0';
                
                memcpy(msg.data, event->data, event->data_len);
                msg.data[event->data_len] = '\0';

                // 发送到队列（等待10 ticks如果队列满）
                if (xQueueSend(MQTT_MessageQueue, &msg, pdMS_TO_TICKS(10)) != pdPASS) {
                    ESP_LOGE(TAG, "Queue full, message dropped");
                    free(msg.topic);
                    free(msg.data);
                }
            }
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            break;
    }
}

void MQTT_Init(QueueHandle_t queue) {
    esp_mqtt_client_config_t mqtt_cfg = {0};
    mqtt_cfg.broker.address.uri = MQTT_URL;
    mqtt_cfg.credentials.client_id = MQTT_KEY;
    // 设置mqtt配置，返回mqtt操作句柄
    s_mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    // 注册mqtt事件回调函数
    esp_mqtt_client_register_event(s_mqtt_client, ESP_EVENT_ANY_ID, MQTT_event_handler, s_mqtt_client);
    // 启动mqtt连接
    esp_mqtt_client_start(s_mqtt_client);

    // 设置消息队列
    MQTT_MessageQueue = queue;
}

bool MQTT_GetConectedState(void) {
    return s_is_mqtt_connected;
}

void MQTT_SendData(char *data) {
    if (s_is_mqtt_connected == true) {
        esp_mqtt_client_publish(s_mqtt_client, MQTT_SENDTOPIC, data, strlen(data), 1, 0);
    }
}