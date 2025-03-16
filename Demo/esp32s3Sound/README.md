# esp32s3Sound

## 基本信息

- 功能：使用ESP32s3采集INMP441的语音数据并使用发送，使用python播放

- 核心板：ESP32s3-WROOM-1（N16R8）



## 使用方法

- 在 `components/WIFI/include/WIFI_STA.h` 文件中填写WIFI信息
- 在 `components/WIFI/include/UDP.h` 文件中填写主机IP地址
- 使用Python运行 `SoundStream.py` 程序