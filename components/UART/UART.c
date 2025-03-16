#include "UART.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#define RX_BUF_SIZE 1024

#define UART_NUM UART_NUM_1

#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_5)

uint8_t* UART_Data;

void UART_Init(void) {
    uart_config_t uart_config = {
        .baud_rate = 115200,    // 波特率
        .data_bits = UART_DATA_8_BITS,    // 数据位
        .parity = UART_PARITY_DISABLE,    // 无奇偶校验
        .stop_bits = UART_STOP_BITS_1,    // 停止位
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,  // 硬件流控制
        .source_clk = UART_SCLK_DEFAULT,
    };

    uart_driver_install(UART_NUM, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    
    UART_Data = (uint8_t*)malloc(RX_BUF_SIZE+1);
}

void UART_Uninstall(void) {
    uart_driver_delete(UART_NUM);
    free(UART_Data);
}

void UART_SendData(uint8_t* data, uint16_t length) {
    uart_write_bytes(UART_NUM, (const char*)data, length);
}

void UART_ReceiveData(uint8_t* data, uint16_t* length) {
    *length = (uint16_t)uart_read_bytes(UART_NUM, (char*)data, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
}
