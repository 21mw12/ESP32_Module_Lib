#include "Button.h"
#include "esp_log.h"

static const char *TAG = "Button";

// 按键消息队列
static QueueHandle_t Button_MessageQueue = NULL;

void Button_Detection_Task(void *arg) {
    int button_state = 0;
    int button_debounce = 0;
    int button_pressed = 0;

    uint32_t Button_Key = 0;

    while (1) {
        button_state = gpio_get_level(Button_Pin);
        if (button_state == 0) {
            button_debounce++;
            if (button_debounce >= 10) {
                if (!button_pressed) {

					// ESP_LOGI(TAG,"Button Pressed!\n");
                    Button_Key = Button_Pin;
                    // 发送到队列（等待10 ticks如果队列满）
                    if (xQueueSend(Button_MessageQueue, &Button_Key, pdMS_TO_TICKS(10)) != pdPASS) {
                        ESP_LOGE(TAG, "Queue full, message dropped");
                    }

                    button_pressed = 1;
                }
            }
        } else {
            button_debounce = 0;
            button_pressed = 0;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void Button_Init(QueueHandle_t queue) {
    gpio_config_t io_conf = {};
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = 1<<Button_Pin;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    
    // 设置消息队列
    Button_MessageQueue = queue;

    xTaskCreate(Button_Detection_Task, "Button_Detection_Task", 2048, NULL, 10, NULL);
}