#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "encoder.h"
#include "driver/gpio.h"

#define ENCODER_A_PIN       (27)
#define ENCODER_B_PIN       (14)
#define ENCODER_S_PIN       (15)
#define DEBOUNCE_CNT        (3)

static const char *TAG = "Encoder";
static SemaphoreHandle_t encoder_slided_sem;

static void IRAM_ATTR
encoder_gpio_isr_handler(void *arg)
{
    int do_yield = pdFALSE;

    if (xSemaphoreGiveFromISR(encoder_slided_sem, &do_yield)) {
        gpio_intr_disable(ENCODER_A_PIN);
    }

    if (do_yield) {
        portYIELD_FROM_ISR();
    }
}

/**
 * @brief Process encoder interrupts. Detect which operation is done.
 * 
 * @param arg 
 */
void 
encoder_task_entry(void *arg)
{
    uint8_t state;
    uint8_t cnt = 0;
    uint8_t direction;
    uint8_t debounce = 0;
    gpio_config_t gpio_params;

    encoder_slided_sem = xSemaphoreCreateBinary();
    if (!encoder_slided_sem) {
        ESP_LOGE(TAG, "fail to create sem\n");
        vTaskDelete(NULL);
        return;
    }

    gpio_params.pin_bit_mask = BIT(ENCODER_A_PIN) | BIT(ENCODER_B_PIN) | BIT(ENCODER_S_PIN);
    gpio_params.mode = GPIO_MODE_INPUT;
    gpio_params.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_params.pull_up_en   = GPIO_PULLUP_ENABLE;
    gpio_params.intr_type    = GPIO_INTR_DISABLE;
    gpio_config(&gpio_params);

    gpio_install_isr_service(0);
    gpio_set_intr_type(ENCODER_A_PIN, GPIO_INTR_POSEDGE);
    gpio_isr_handler_add(ENCODER_A_PIN, encoder_gpio_isr_handler, NULL);

    while (true) {
        gpio_intr_enable(ENCODER_A_PIN);
        /* Wait for interrupts */
        if (!xSemaphoreTake(encoder_slided_sem, portMAX_DELAY)) {
            continue;
        }

        debounce = 0;
        
        while (1) {
            direction = gpio_get_level(ENCODER_B_PIN);
            state = gpio_get_level(ENCODER_A_PIN);
            /* Check state multiple timers to prevent jitter */
            if (state) {
                ++debounce;

                if (debounce >= DEBOUNCE_CNT) {
                    if (direction) {
                        ++cnt;
                    } else {
                        --cnt;
                    }
                    printf("cnt:%d\n", cnt);
                    break;
                }
                vTaskDelay(2);
            } else {
                break;
            }
        }
    }
}