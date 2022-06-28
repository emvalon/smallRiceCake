#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "encoder.h"
#include "gui.h"
#include "webServer.h"
#include "esp_ota_ops.h"
#include "btSink.h"
#include "battery.h"

static const char *TAG = "main";

void app_main(void)
{
    esp_err_t ret;

    /* Initialize NVS */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);


    /* Encoder task */
    ret = xTaskCreatePinnedToCore(encoder_task_entry, "encoder task", 
                                  1024 * 2, NULL, configMAX_PRIORITIES - 1, 
                                  NULL, 1);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "fail to create encoder task, %d", ret);
    }
    /* Gui task */
    ret = xTaskCreatePinnedToCore(gui_task_entry, "gui task", 
                                  1024 * 6, NULL, 2, 
                                  NULL, 1);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "fail to create gui task, %d", ret);
    }
     /* Battery task */
    ret = xTaskCreatePinnedToCore(battery_task_entry, "gui task", 
                                  1024 * 4, NULL, 3, 
                                  NULL, 1);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "fail to create battery task, %d", ret);
    }
    /* btSink task */
    ret = xTaskCreatePinnedToCore(btSink_task_entry, "btSink task", 
                                  1024 * 4, NULL, configMAX_PRIORITIES - 3, 
                                  NULL, 1);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "fail to create btSink task, %d", ret);
    }
    
    /* web server task */
    webServer_task_entry(NULL);

    esp_ota_mark_app_valid_cancel_rollback();
}
