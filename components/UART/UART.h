#ifndef __UART_H_
#define __UART_H_

#include <stdio.h>

/* 串口接收缓存数组 */
extern uint8_t* UART_Data;

void UART_Init(void);

void UART_Uninstall(void);

void UART_SendData(uint8_t* data, uint16_t length);

void UART_ReceiveData(uint8_t* data, uint16_t* length);

#endif
