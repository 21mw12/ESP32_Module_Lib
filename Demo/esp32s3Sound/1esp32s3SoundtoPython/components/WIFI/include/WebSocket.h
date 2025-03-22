#ifndef __WEB_SOCKET_H_
#define __WEB_SOCKET_H_

#include <string.h>

/* 缓冲区大小 */
#define BUFFER_SIZE 512

/* WebSocket服务器的URI */
#define WEBSOCKET_URI "ws://192.168.31.178:27000/ws/transcribe?lang=auto"

/**
  * @brief  初始化WebSocket客户端
  * @retval 无
  */
void WebSocket_Init(void);

/**
  * @brief  发送二进制数据
  * @param  data_byte_stream 二进制数据流
  * @retval 无
  */
void WebSocket_Sent(int16_t *data_byte_stream);

#endif