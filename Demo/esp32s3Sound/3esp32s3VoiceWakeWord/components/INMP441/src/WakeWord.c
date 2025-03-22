#include "WakeWord.h"
#include "INMP441.h"
#include "esp_log.h"
#include "esp_err.h"

#include "esp_mn_models.h"
#include "model_path.h"
#include "esp_mn_iface.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "esp_afe_sr_models.h"
#include "esp_afe_sr_iface.h"
#include "esp_mn_speech_commands.h"

/* 日志所属 */
static const char *TAG = "WakeWord";

typedef struct {
    wakenet_state_t     wakenet_mode;
    esp_mn_state_t      state;
    int                 command_id;
} sr_result_t;

static QueueHandle_t command_result_queue = NULL;   // 识别到的命令结果队列
static esp_afe_sr_iface_t *afe_handle = NULL;       // AFE处理句柄
static model_iface_data_t *model_data = NULL;       // 模型数据句柄
static const esp_mn_iface_t *multinet = NULL;       // 多神经网络接口
int detect_flag = 0;        // 唤醒词检测标志位

/* 唤醒命令语句，使用拼音中间空格 */
const char *command_trigger_sentence[12] = {
    "da kai kong qi jing hua qi",
    "guan bi kong qi jing hua qi",
    "da kai tai deng",
    "guan bi tai deng",
    "tai deng tiao liang",
    "tai deng tiao an",
    "da kai deng dai",
    "guan bi deng dai",
    "bo fang yin yue",
    "ting zhi bo fang",
    "da kai shi jian",
    "da kai ri li"
};

/**
  * @brief  数据输入任务，用于读取INMP441传感器的数据并将其传递给AFE处理
  * @param  arg AFE数据结构体指针
  */
void SR_Feed_Task(void *arg) {
    // 将传入的参数arg转换为esp_afe_sr_data_t类型的指针afe_data
    esp_afe_sr_data_t *afe_data = arg;

    while (1) {
        // 读取INMP441传感器的数据
        INMP441_Read();

        // 将读取到的数据传递给AFE处理
        afe_handle->feed(afe_data, INMP441_sBuffer);
    }
    vTaskDelete(NULL);
}

/**
  * @brief  唤醒词输出任务，将唤醒词检测模块的结果输出到串口
  * @param  arg AFE数据结构体指针
  */
void SR_Detect_Task(void *arg) {
    // 定义一个指向esp_afe_sr_data_t结构体的指针，用于存储AFE数据
    esp_afe_sr_data_t *afe_data = arg;
    // 获取AFE数据块的大小
    int afe_chunksize = afe_handle->get_fetch_chunksize(afe_data);
    // 分配一个int16_t类型的缓冲区，用于存储AFE数据块
    int16_t *buff = malloc(afe_chunksize * sizeof(int16_t));
    assert(buff);

    printf("------------DETECT START------------\n");

    while (1) {
        // 获取AFE数据块
        afe_fetch_result_t* res = afe_handle->fetch(afe_data);
        if (!res || res->ret_value == ESP_FAIL) {
            ESP_LOGE(TAG, "cfetch error!\n");
            break;
        }

        // 如果检测到唤醒词，则打印唤醒词的索引，并设置检测标志为true
        if (res->wakeup_state == WAKENET_DETECTED) {
	        ESP_LOGI("Detected WakeUp - model index:%d, word index:%d\n", res->wakenet_model_index, res->wake_word_index);
            printf("------------LISTENING------------\n");
            sr_result_t result = {
                .wakenet_mode = WAKENET_DETECTED,
                .state = ESP_MN_STATE_DETECTING,
                .command_id = 0,
            };
            xQueueSend(command_result_queue, &result, 10);
            detect_flag = true;
        // 如果验证通道，则设置检测标志为true，并禁用唤醒词检测
        } else if (res->wakeup_state == WAKENET_CHANNEL_VERIFIED) {
            afe_handle->disable_wakenet(afe_data);
            detect_flag = true;
        }

        // 如果检测标志为true，则进行多网络检测
        if (true == detect_flag) {
            esp_mn_state_t mn_state = ESP_MN_STATE_DETECTING;
            mn_state = multinet->detect(model_data, res->data);

            // 如果检测状态为检测中，则继续检测
            if (mn_state == ESP_MN_STATE_DETECTING) {
                continue;
            }

            // 如果检测超时，则打印超时信息，并发送超时结果，并重新启用唤醒词检测
            else if (mn_state == ESP_MN_STATE_TIMEOUT) {
                ESP_LOGW(TAG, "Detected Time out");
                sr_result_t result = {
                    .wakenet_mode = WAKENET_NO_DETECT,
                    .state = mn_state,
                    .command_id = 0,
                };
                xQueueSend(command_result_queue, &result, 10);
                afe_handle->enable_wakenet(afe_data);
                detect_flag = false;
                continue;
            }

            // 如果检测到指令，则打印指令信息，并发送指令结果
            else if (mn_state == ESP_MN_STATE_DETECTED) {
                esp_mn_results_t *mn_result = multinet->get_results(model_data);
                for (int i = 0; i < mn_result->num; i++) {
                    ESP_LOGI(TAG, "TOP %d, command_id: %d, phrase_id: %d, prob: %f",
                            i + 1, mn_result->command_id[i], mn_result->phrase_id[i], mn_result->prob[i]);
                }
                int sr_command_id = mn_result->command_id[0];
                ESP_LOGI(TAG, "Deteted command : %d", sr_command_id);
                sr_result_t result = {
                    .wakenet_mode = WAKENET_NO_DETECT,
                    .state = mn_state,
                    .command_id = sr_command_id,
                };
                xQueueSend(command_result_queue, &result, 10);
            }
        }        
    }
    if (buff) {
        free(buff);
        buff = NULL;
    }
    vTaskDelete(NULL);
}

void SR_handler_task(void *pvParam) {
    // 定义一个队列句柄，用于接收参数
    QueueHandle_t xQueue = (QueueHandle_t) pvParam;

    while (true) {
        // 定义一个sr_result_t类型的变量，用于接收队列中的数据
        sr_result_t result;
        // 从队列中接收数据，阻塞等待
        xQueueReceive(xQueue, &result, portMAX_DELAY);

        // 打印接收到的数据
        ESP_LOGI(TAG, "cmd:%d, wakemode:%d,state:%d", result.command_id, result.wakenet_mode, result.state);

        // 如果接收到的状态为超时，则打印超时信息
        if (ESP_MN_STATE_TIMEOUT == result.state) {
            ESP_LOGI(TAG, "timeout");
            continue;
        }

        // 如果接收到的唤醒模式为WAKENET_DETECTED，则打印唤醒信息
        if (WAKENET_DETECTED == result.wakenet_mode) {
            ESP_LOGI(TAG, "wakenet detected %d", result.command_id);
            continue;
        }
    }
}

void WakeWord_Init(void) {
    // 初始化唤醒词模型列表
    srmodel_list_t *models = esp_srmodel_init("model");
    // 初始化AFE配置
    afe_config_t *afe_config = afe_config_init("M", models, AFE_TYPE_SR, AFE_MODE_LOW_COST);
    
    // 设置唤醒词模型名称
    afe_config->wakenet_model_name = esp_srmodel_filter(models, ESP_WN_PREFIX, NULL);
    // 设置AEC初始化为false
    afe_config->aec_init = false;
    // 从配置中创建AFE句柄
    afe_handle = esp_afe_handle_from_config(afe_config);
    // 从配置中创建AFE数据
    esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(afe_config);
    
    // 获取多网络模型名称
    char *mn_name = esp_srmodel_filter(models, ESP_MN_CHINESE, NULL);
    // 如果没有找到多网络模型，则打印提示信息并返回
    if (NULL == mn_name) {
        printf("No multinet model found");
        return ;
    }
    // 从名称中创建多网络句柄
    multinet = esp_mn_handle_from_name(mn_name);
    // 从名称中创建多网络数据
    model_data = multinet->create(mn_name, 5760);//设置唤醒超时时间
    printf( "load multinet:%s", mn_name);
    esp_mn_commands_clear();//清除唤醒指令
    for (int i = 0; i < sizeof(command_trigger_sentence) / sizeof(command_trigger_sentence[0]); i++) {
        esp_mn_commands_add(i, (char *)command_trigger_sentence[i]);//逐个将唤醒指令放入
    }
    esp_mn_commands_update();//更新命令词列表
    esp_mn_commands_print();
    multinet->print_active_speech_commands(model_data);//输出目前激活的命令词
    
    afe_config_free(afe_config);
    
    command_result_queue = xQueueCreate(1, sizeof(sr_result_t));

    xTaskCreatePinnedToCore(&SR_Feed_Task, "feed", 8 * 1024, (void*)afe_data, 5, NULL, 0);
    xTaskCreatePinnedToCore(&SR_Detect_Task, "detect", 4 * 1024, (void*)afe_data, 5, NULL, 1);
    xTaskCreatePinnedToCore(&SR_handler_task, "SR Handler Task", 4 * 1024, command_result_queue, 1, NULL, 1);

}
