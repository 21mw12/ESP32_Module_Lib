#ifndef __WIFI_HTTPT_H
#define __WIFI_HTTPT_H

#include "esp_http_client.h"
#include "esp_http_server.h"
#include "cJSON.h"

/**
  * @brief 初始化 HTTP 客户端
  * @return 无
 */
void HTTP_Init(void);

/**
  * @brief  发送 HTTP GET 请求
  * @param  url 请求的 URL
  * @retval 无
  */
cJSON* HTTP_Get_Request(const char *url);

/**
  * @brief  发送 HTTP GET 请求
  * @param  url 请求的 URL
  * @param  json_data 请求的 JSON 数据
  * @retval 无
  */
cJSON* HTTP_Post_Request(const char *url, const char *json_data);

/**
  * @brief  启动HTTP服务器(目前配合ESP32CAM使用)
  * @param  uri URL地址
  * @param  handler 处理函数
  * @retval 无
  */
void HTTP_Start_WebServer(char* uri, esp_err_t (*handler)(httpd_req_t *));

/**
发送POST请求示例：
    // 构造JSON数据
    cJSON *json_root = cJSON_CreateObject();
    cJSON_AddStringToObject(json_root, "filename", "ESP32");
    cJSON_AddNumberToObject(json_root, "value", 123);
    char *post_data = cJSON_PrintUnformatted(json_root);
    // 发送POST请求
    cJSON* json_response = HTTP_Post_Request("http://192.168.31.178:5000/save_json", post_data);
    // 处理响应
    if (json_response == NULL) {
        ESP_LOGE(TAG, "Failed to parse JSON response");
    } else {
        // 提取并显示字段内容
        cJSON *response_field = cJSON_GetObjectItem(json_response, "response");
        if (response_field != NULL) {
            ESP_LOGI(TAG, "Response content: %s", response_field->valuestring);
        } else {
            ESP_LOGE(TAG, "Field 'response' not found in JSON");
        }
        cJSON_Delete(json_response);
    }
    // 释放资源
    cJSON_Delete(json_root);
    free(post_data);
*/

#endif
