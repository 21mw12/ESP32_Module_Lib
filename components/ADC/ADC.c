// #include "ADC.h"
// #include "esp_adc/adc_oneshot.h"
// #include "esp_adc/adc_cali.h"
// #include "esp_adc/adc_cali_scheme.h"
// #include "esp_log.h"

// adc_oneshot_unit_handle_t adc2_handle;

// void ADC_Init(void) {
//      //-------------ADC2 Init---------------//
//     adc_oneshot_unit_init_cfg_t init_config2 = {
//         .unit_id = ADC_UNIT_2,
//         .ulp_mode = ADC_ULP_MODE_DISABLE,
//     };
//     ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config2, &adc2_handle));

//     // //-------------ADC2 Calibration Init---------------//
//     // adc_cali_handle_t adc2_cali_handle = NULL;
//     // bool do_calibration2 = example_adc_calibration_init(ADC_UNIT_2, ADC_CHANNEL_3, ADC_ATTEN_DB_11, &adc2_cali_handle);

//     //-------------ADC2 Config---------------//
//     adc_oneshot_chan_cfg_t config = {
//         .bitwidth = ADC_BITWIDTH_DEFAULT,
//         .atten = ADC_ATTEN_DB_11,
//     };
//     ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, ADC_CHANNEL_3, &config));
// }

// int ADC_Read(void) {
//     int adc_reading;
//     adc_oneshot_read(adc2_handle, ADC_CHANNEL_3, &adc_reading);
//     return adc_reading;
// }

#include "ADC.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_continuous.h"
#include "esp_err.h"

#define Debug_ADC       0

static adc_channel_t channel[6] = {
    ADC_CHANNEL_0,      // GPIO36
    ADC_CHANNEL_3,      // GPIO39
    ADC_CHANNEL_4,      // GPIO32
    ADC_CHANNEL_5,       // GPIO33
    ADC_CHANNEL_6,      // GPIO34
    ADC_CHANNEL_7,      // GPIO35
};

static uint8_t ChannelToIndex[][2] = {
    {0, 0},
    {3, 1},
    {4, 2},
    {5, 3},
    {6, 4},
    {7, 5},
};

#define Average_Base        4
#define Channel_Count       sizeof(channel) / sizeof(adc_channel_t)
#define READ_LEN            2 * Channel_Count * Average_Base
uint8_t originalData[READ_LEN] = {0};

static TaskHandle_t s_task_handle;
adc_continuous_handle_t handle;

static bool IRAM_ATTR ADC_ISR(const adc_continuous_evt_data_t *edata, void *user_data) {
    BaseType_t mustYield = pdFALSE;
    vTaskNotifyGiveFromISR(s_task_handle, &mustYield);
    return (mustYield == pdTRUE);
}

void ADC_Init(void) {
    s_task_handle = xTaskGetCurrentTaskHandle();
    memset(originalData, 0xcc, READ_LEN);

    /* 连续模式配置 */
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 1024,
        .conv_frame_size = READ_LEN,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));

    /* ADC配置 */
    adc_continuous_config_t dig_cfg;
    dig_cfg.pattern_num = Channel_Count;           // 开启的通道数量
    dig_cfg.conv_mode = ADC_CONV_SINGLE_UNIT_1;    // 仅使用ADC1进行转换
    dig_cfg.format = ADC_DIGI_OUTPUT_FORMAT_TYPE1; // 使用type1类型的数据结构
    dig_cfg.sample_freq_hz = 20 * 1000;            // ADC采样率
    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};  // 每个通道的配置
    for (uint8_t i = 0; i < dig_cfg.pattern_num; i++) {
        adc_pattern[i].atten = ADC_ATTEN_DB_11;
        adc_pattern[i].channel = channel[i] & 0x7;
        adc_pattern[i].unit = ADC_UNIT_1;
        adc_pattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
    }
    dig_cfg.adc_pattern = adc_pattern;
    ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg));

    /* ADC回调函数配置 */
    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = ADC_ISR,
    };
    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(handle, &cbs, NULL));

    /* 开启ADC转换 */
    ESP_ERROR_CHECK(adc_continuous_start(handle));
}

void ADC_Read(uint16_t* data) {
    uint32_t length = 0;
    adc_continuous_read(handle, originalData, READ_LEN, &length, 0);
    for (int i = 0; i < READ_LEN; i+=SOC_ADC_DIGI_RESULT_BYTES) {
        adc_digi_output_data_t *p = (void*)&originalData[i];
        if (p->type1.channel < SOC_ADC_CHANNEL_NUM(ADC_UNIT_1)) {
#if Debug_ADC == 1
            printf("channel[%d] = %d\r\n", p->type1.channel, p->type1.data);
#endif
            for(uint8_t i = 0; i < Channel_Count; i++) {
                if(ChannelToIndex[i][0] == p->type1.channel) {
                    // 将通道数据存储到数组中
                    data[ChannelToIndex[i][1]] = p->type1.data;
                }
            }
        }
    }
    vTaskDelay(50);
}

// uint16_t ADC_GetValue(adc_channel_t channel) {
//     uint16_t result = 0;
//     for (int i = 0; i < READ_LEN; i+=SOC_ADC_DIGI_RESULT_BYTES) {
//         adc_digi_output_data_t *p = (void*)&originalData[i];
//         if (p->type1.channel < SOC_ADC_CHANNEL_NUM(ADC_UNIT_1)) {
//             printf("channel[%d] = %d\r\n", p->type1.channel, p->type1.data);
//         }
//     }
//     return result;
// }

void ADC_DeInit(void) {
    ESP_ERROR_CHECK(adc_continuous_stop(handle));
    ESP_ERROR_CHECK(adc_continuous_deinit(handle));
}