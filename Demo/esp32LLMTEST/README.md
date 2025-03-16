# esp32LLMTEST

## 基本信息

- 功能：使用ESP32的HTTP请求向大语言模型，并讲接收到的数据打印

- 核心板：ESP32-WROOM
- LLM：ollama本地部署



## 使用方法

- 在 `components/WIFI/include/WIFI_STA.h` 文件中填写WIFI信息
- 修改 `main/esp32-test.c` 中的请求地址，并从新编辑请求体内容