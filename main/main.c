#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "encoder.h"
#include "gui.h"

static const char *TAG = "Encoder";

void app_main(void)
{
    BaseType_t res;

    /* Encoder task */
    res = xTaskCreatePinnedToCore(encoder_task_entry, "encoder task", 
                                  1024 * 2, NULL, configMAX_PRIORITIES - 1, 
                                  NULL, 1);
    if (res != pdPASS) {
        ESP_LOGE(TAG, "fail to create encoder task, %d", res);
    }
    /* Gui task */
    res = xTaskCreatePinnedToCore(gui_task_entry, "gui task", 
                                  1024 * 4, NULL, 1, 
                                  NULL, 1);
    if (res != pdPASS) {
        ESP_LOGE(TAG, "fail to create gui task, %d", res);
    }
}
