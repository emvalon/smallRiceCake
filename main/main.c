#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"




void app_main(void)
{
    gpio_config_t gpio_params;

    //codec chip power enable
    gpio_params.pin_bit_mask = BIT22;
    gpio_params.mode = GPIO_MODE_OUTPUT;
    gpio_params.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_params.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_params.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&gpio_params);
    gpio_set_level(GPIO_NUM_22, 1);

    // gpio_set_direction(18, GPIO_MODE_OUTPUT);
    // gpio_set_level(18, 1);

    // gpio_set_direction(23, GPIO_MODE_OUTPUT);
    // gpio_set_level(23, 1);

    while(1) {
        printf("hello word\n");
        vTaskDelay(100);
    }
}
