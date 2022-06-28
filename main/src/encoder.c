#include "sdkconfig.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "encoder.h"
#include "driver/gpio.h"

#include "flexible_button.h"
#include "vlonGui_input.h"

#define ENCODER_A_PIN       (27)
#define ENCODER_B_PIN       (14)
#define ENCODER_S_PIN       (15)
#define DEBOUNCE_CNT        (3)
#define ENCODER_BTN_SCAN_INTV_MS        (1000 / FLEX_BTN_SCAN_FREQ_HZ)

static const char *TAG = "Encoder";
static SemaphoreHandle_t encoder_slided_sem;

static flex_button_t user_button;

static uint8_t 
encoder_btn_read(void *arg)
{
    return gpio_get_level(ENCODER_S_PIN);
}

static void 
encoder_btn_evt_cb(void *arg)
{
    flex_button_event_t evt;
    flex_button_t *btn;
    
    btn = (flex_button_t *)arg;
    evt = flex_button_event_read(btn);

    switch (evt)
    {
    case FLEX_BTN_PRESS_CLICK:
        vlonGui_inputEnqueueKey(VLGUI_KEY_OK);
        break;
    case FLEX_BTN_PRESS_DOUBLE_CLICK:
        vlonGui_inputEnqueueKey(VLGUI_KEY_ESC);
        break;
    default:
        break;
    }
}


static void 
encoder_button_init(void)
{  
    memset(&user_button, 0x0, sizeof(user_button));

    user_button.id = 0;
    user_button.usr_button_read = encoder_btn_read;
    user_button.cb = encoder_btn_evt_cb;
    user_button.pressed_logic_level = 0;
    user_button.short_press_start_tick = FLEX_MS_TO_SCAN_CNT(1500);
    user_button.long_press_start_tick = FLEX_MS_TO_SCAN_CNT(3000);
    user_button.long_hold_start_tick = FLEX_MS_TO_SCAN_CNT(4500);
    user_button.pressed_logic_level = 0;

    flex_button_register(&user_button);
}

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
    uint32_t now;
    uint8_t state;
    uint8_t cnt = 0;
    uint8_t direction;
    uint32_t last_time;
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

    encoder_button_init();

    last_time = 0;
    while (true) {
        now = xTaskGetTickCount();
        if ((now - last_time) >= ENCODER_BTN_SCAN_INTV_MS) {
            flex_button_scan();
            last_time = now;
        }

        gpio_intr_enable(ENCODER_A_PIN);
        /* Wait for interrupts */
        if (!xSemaphoreTake(encoder_slided_sem, ENCODER_BTN_SCAN_INTV_MS)) {
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
                        vlonGui_inputEnqueueKey(VLGUI_KEY_RIGHT);
                    } else {
                        vlonGui_inputEnqueueKey(VLGUI_KEY_LETF);
                        --cnt;
                    }
                    break;
                }
                vTaskDelay(1);
            } else {
                break;
            }
        }
    }
}