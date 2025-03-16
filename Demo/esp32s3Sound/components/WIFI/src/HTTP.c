#include "HTTP.h"
#include "esp_log.h"
#include "esp_http_client.h"

static const char *TAG = "HTTP";

// 全局变量，用于存储响应数据
static char response_buffer[1024];
static int response_length = 0;

// HTTP事件处理函数
static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // 将接收到的数据存储到缓冲区
                memcpy(response_buffer + response_length, evt->data, evt->data_len);
                response_length += evt->data_len;
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            // 响应接收完成，确保缓冲区以 null 结尾
            response_buffer[response_length] = '\0';
            ESP_LOGI(TAG, "Response data: %s", response_buffer);
            break;
        default:
            break;
    }
    return ESP_OK;
}

// 初始化 HTTP 客户端
void HTTP_Init(void) {
    // 清空响应缓冲区
    memset(response_buffer, 0, sizeof(response_buffer));
    response_length = 0;
}

// 发送 HTTP GET 请求
cJSON* HTTP_Get_Request(const char *url) {
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handler,
        .method = HTTP_METHOD_GET,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // 清空响应缓冲区
    memset(response_buffer, 0, sizeof(response_buffer));
    response_length = 0;

    // 发送请求
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP Status = %d, content_length = %lld",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));

        // 解析响应数据
        cJSON *json_response = cJSON_Parse(response_buffer);
        esp_http_client_cleanup(client);
        return json_response;
    }

    ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    // 清理 HTTP 客户端
    esp_http_client_cleanup(client);
    return NULL;
}

// 发送 HTTP POST 请求
cJSON* HTTP_Post_Request(const char *url, const char *json_data) {
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handler,
        .method = HTTP_METHOD_POST,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // 设置 POST 数据和头部
    esp_http_client_set_post_field(client, json_data, strlen(json_data));
    esp_http_client_set_header(client, "Content-Type", "application/json");

    // 清空响应缓冲区
    memset(response_buffer, 0, sizeof(response_buffer));
    response_length = 0;

    // 发送请求
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP Status = %d, content_length = %lld",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));

        // 解析响应数据
        cJSON *json_response = cJSON_Parse(response_buffer);
        esp_http_client_cleanup(client);
        return json_response;
    }

    ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    // 清理 HTTP 客户端
    esp_http_client_cleanup(client);
    return NULL;
}

void HTTP_Start_Webserver(char* uri, esp_err_t (*handler)(httpd_req_t *)) {
    static httpd_handle_t server = NULL;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t web_uri = {
            .uri = uri,
            .method = HTTP_GET,
            .handler = handler,
        };
        httpd_register_uri_handler(server, &web_uri);
        ESP_LOGI(TAG, "HTTP server started with MJPEG stream");
    } else {
        ESP_LOGE(TAG, "Failed to start HTTP server");
    }
}
