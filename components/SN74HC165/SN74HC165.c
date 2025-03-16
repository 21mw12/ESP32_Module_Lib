#include "SN74HC165.h"
#include "driver/gpio.h"
#include "esp_log.h"

uint8_t SN74HC165Data[NUM_OF_74HC165];

void SN74HC165_Init(void) {
    gpio_config_t io_conf;

    // 配置SER_OUT为输入
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << PIN_SER_OUT);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // 配置CLK和SH/LD为输出
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << PIN_CLK) | (1ULL << PIN_SH_LD);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
}

void SN74hc165_read(void) {
    // 装载数据（SH/LD低电平）
    gpio_set_level(PIN_SH_LD, 0);
    esp_rom_delay_us(1);  // 短暂延时
    gpio_set_level(PIN_SH_LD, 1);

    // 逐芯片读取数据
    for (int chip = 0; chip < NUM_OF_74HC165; chip++) {
        SN74HC165Data[chip] = 0;  // 初始化当前芯片的数据

        // 逐位读取数据
        for (int i = 0; i < 8; i++) {
            SN74HC165Data[chip] |= gpio_get_level(PIN_SER_OUT) << (7 - i);

            // 产生时钟上升沿
            gpio_set_level(PIN_CLK, 1);
            esp_rom_delay_us(1);  // 短暂延时
            gpio_set_level(PIN_CLK, 0);
        }
    }
}