# 使用说明

- 拉取esp32-camera

  ```shell
  idf.py add-dependency espressif/esp32-camera
  ```

- 需要配合WIFI的HTTP，并传入对应的处理函数（图片或视频流）

  ```c
  void app_main(void) {
      esp_err_t ret = nvs_flash_init();
      if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
          ESP_ERROR_CHECK(nvs_flash_erase());
          ret = nvs_flash_init();
      }
      ESP_ERROR_CHECK(ret);
  
      WIFI_STA_Init();
  
      if (Camera_Init() != ESP_OK) {
          return;
      }
  
      HTTP_Start_Webserver("/esp32", Camera_jpg_Stream_Handler);
  }
  ```

- 可通过浏览器获取视频流或python