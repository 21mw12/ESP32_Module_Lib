file(GLOB SRCS "src/*.c")

set(srcs ${SRCS})

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include"
                    REQUIRES "esp_wifi" "esp_http_client" "esp_http_server" "cJSON")