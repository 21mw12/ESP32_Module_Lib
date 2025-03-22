#include "LED.h"
#include "driver/gpio.h"

uint32_t LED_State = 0;

void LED_Init(void) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;      // 关闭中断
    io_conf.mode = GPIO_MODE_OUTPUT;            // 输出模式
    io_conf.pin_bit_mask = 1<<LED_Pin;          // 设置引脚
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;   // 上拉电阻
    io_conf.pull_down_en = GPIO_PULLUP_ENABLE;  // 下拉电阻
    gpio_config(&io_conf);

    LED_SetLow();
}

void LED_Toggle(void) {
    LED_State = !LED_State;
    gpio_set_level(LED_Pin, LED_State);
}

void LED_SetHeight(void) {
    LED_State = 1;
    gpio_set_level(LED_Pin, LED_State);
}

void LED_SetLow(void) {
    LED_State = 0;
    gpio_set_level(LED_Pin, LED_State);
}
