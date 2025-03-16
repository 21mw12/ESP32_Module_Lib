set(srcs "src/WIFI_STA.c" "src/MQTT.c" "src/HTTP.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include"
                    REQUIRES "esp_wifi" "mqtt" "esp_http_client" "cJSON")