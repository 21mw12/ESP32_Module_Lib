idf_component_register(
    SRCS "Camera.c"
    INCLUDE_DIRS "."
    REQUIRES "espressif__esp32-camera" "esp_http_server" "esp_timer"
)