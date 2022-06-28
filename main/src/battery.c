#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc_common.h"
#include "esp_adc_cal.h"
#include "battery.h"
#include "gui.h"

static const char *TAG = "BATTERY";

static esp_adc_cal_characteristics_t adc1_chars;
static uint8_t level;

static bool 
adc_calibration_init(void)
{
    esp_err_t ret;
    bool cali_enable = false;

    ret = esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF);
    if (ret == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGW(TAG, "Calibration scheme not supported, skip software calibration");
    } else if (ret == ESP_ERR_INVALID_VERSION) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else if (ret == ESP_OK) {
        cali_enable = true;
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
    } else {
        ESP_LOGE(TAG, "Invalid arg");
    }

    return cali_enable;
}

void 
battery_task_entry(void *arg)
{
    uint32_t voltage = 0;
    uint32_t old_voltage = 0;
    bool cali_enable = adc_calibration_init();
    uint8_t message[3];
    uint8_t new_level;

    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11));

    message[0] = GUI_MESSAGE_BATTERY;


    vTaskDelay(pdMS_TO_TICKS(100));

    while (esp_adc_cal_get_voltage(ADC1_CHANNEL_0, &adc1_chars, &old_voltage) != ESP_OK);
    if (old_voltage >= 1960) {
        level = 3;
    } else if (old_voltage >= 1895) {
        level = 2;
    } else if (old_voltage >= 1840) {
        level = 1;
    } else {
        level = 0;
    }

    message[1] = level;
    message[2] = 0;
    if (!xMessageBufferSend(gui_message, message, sizeof(message), 0)) {
        ESP_LOGE(TAG, "failed to send message");
    }

    while (1) {
        if (esp_adc_cal_get_voltage(ADC1_CHANNEL_0, &adc1_chars, &voltage) == ESP_OK) {
            if (voltage <= old_voltage) {
                if (old_voltage >= 1960) {
                    new_level = 3;
                } else if (old_voltage >= 1895) {
                    new_level = 2;
                } else if (old_voltage >= 1840) {
                    new_level = 1;
                } else {
                    new_level = 0;
                }
            } else {
                if (old_voltage >= 1980) {
                    new_level = 3;
                } else if (old_voltage >= 1915) {
                    new_level = 2;
                } else if (old_voltage >= 1860) {
                    new_level = 1;
                } else {
                    new_level = 0;
                }
            }
            if (new_level != level) {
                message[1] = new_level;
                if (!xMessageBufferSend(gui_message, message, sizeof(message), 0)) {
                    ESP_LOGE(TAG, "failed to send message");
                } else {
                    level = new_level;
                    old_voltage = voltage;
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
