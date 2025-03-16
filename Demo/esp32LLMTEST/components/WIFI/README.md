# MQTT

## 使用方法

以在 `main.c` 文件中使用为例

1. 声明MQTT消息队列

   ```c
   QueueHandle_t MATT_Queue;
   ```

2. 编写MQTT消息处理任务

   ```c
   static void MQTT_Message_Handler_Task(void* pvParameters) {
       MQTTMessage_t received_msg;
       
       MATT_Queue = xQueueCreate(10, sizeof(MQTTMessage_t));
       
       WIFI_STA_Init();
       MQTT_Init(MATT_Queue);
       
       while (1) {
           // 阻塞等待队列消息
           if (xQueueReceive(MATT_Queue, &received_msg, portMAX_DELAY)) {
               // 处理消息
               ESP_LOGI(TAG, "Processing message in task: topic=%s, data=%s", received_msg.topic, received_msg.data);
   
               if (strstr(received_msg.topic, MQTT_TOPIC) != NULL) {
                   if (strstr(received_msg.data, 命令1) != NULL) {
                       // 处理命令1
                   } 
                   else if (strstr(received_msg.data, 命令2) != NULL) {
                       // 处理命令2
                   }
               }
   
               // 释放内存
               free(received_msg.topic);
               free(received_msg.data);
           }
       }
   }
   ```

3. 创建MQTT消息处理任务

   ```c
   xTaskCreate(MQTT_Message_Handler_Task, 
               "MQTT_Message_Handler_Task", 
               4096, 
               NULL, 
               configMAX_PRIORITIES - 2,  // 中等优先级
               NULL);
   ```



## 示例

```c
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "nvs_flash.h"
#include "WIFI_STA.h"
#include "MQTT.h"

/* 日志所属 */
static const char *TAG = "APP";

QueueHandle_t MATT_Queue;

static void MQTT_Message_Handler_Task(void* pvParameters) {
    MQTTMessage_t received_msg;
    
    MATT_Queue = xQueueCreate(10, sizeof(MQTTMessage_t));
    
    WIFI_STA_Init();
    MQTT_Init(MATT_Queue);
    
    while (1) {
        // 阻塞等待队列消息
        if (xQueueReceive(MATT_Queue, &received_msg, portMAX_DELAY)) {
            // 处理消息
            ESP_LOGI(TAG, "Processing message in task: topic=%s, data=%s", received_msg.topic, received_msg.data);

            if (strstr(received_msg.topic, MQTT_TOPIC) != NULL) {
                if (strstr(received_msg.data, "on") != NULL) {
                    // 处理on命令
                    ESP_LOGI(TAG, "Received ON command");
                } 
                else if (strstr(received_msg.data, "off") != NULL) {
                    // 处理off命令
                    ESP_LOGI(TAG, "Received OFF command");
                }
            }

            // 释放内存
            free(received_msg.topic);
            free(received_msg.data);
        }
    }
}

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    xTaskCreate(MQTT_Message_Handler_Task, 
                "MQTT_Message_Handler_Task", 
                4096, 
                NULL, 
                configMAX_PRIORITIES - 2,  // 中等优先级
                NULL);
    
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

