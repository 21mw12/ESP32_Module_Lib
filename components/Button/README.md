# FreeRTOS队列按键使用方法

以在 `main.c` 文件中使用为例

1. 声明按键消息队列

   ```c
   QueueHandle_t Button_Queue;
   ```

2. 编写按键消息处理任务

   ```c
   static void Button_Message_Handler_Task(void* pvParameters) {
       uint32_t received_msg;
   
       Button_Queue = xQueueCreate(10, sizeof(uint32_t));
       
       Button_Init(Button_Queue);
   
       while (1) {
           // 阻塞等待队列消息
           if (xQueueReceive(Button_Queue, &received_msg, portMAX_DELAY)) {
               if (received_msg == Button_Pin) {
                   ESP_LOGI(TAG,"Button Pressed!\n");
               }
           }
       }
   }
   
   ```

3. 创建按键消息处理任务

   ```c
   xTaskCreate(Button_Message_Handler_Task, 
               "Button_Message_Handler_Task", 
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
#include "driver/gpio.h"
#include "LED.h"
#include "Button.h"

/* 日志所属 */
static const char *TAG = "APP";

QueueHandle_t Button_Queue;

static void Button_Message_Handler_Task(void* pvParameters) {
    uint32_t received_msg;

    Button_Queue = xQueueCreate(10, sizeof(uint32_t));
    
    LED_Init();
    Button_Init(Button_Queue);

    while (1) {
        // 阻塞等待队列消息
        if (xQueueReceive(Button_Queue, &received_msg, portMAX_DELAY)) {
            if (received_msg == Button_Pin) {
                LED_Toggle();
            }
        }
    }
}


void app_main(void) {
    //初始化NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    xTaskCreate(Button_Message_Handler_Task, 
                "Button_Message_Handler_Task", 
                4096, 
                NULL, 
                configMAX_PRIORITIES - 2,  // 中等优先级
                NULL);
    
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

