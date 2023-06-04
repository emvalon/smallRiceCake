#include "sleep.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp_log.h"

#define TAG                     "SLEEP"
#define SLEEP_WAKEUP_PIN        (15)

void
deep_sleep_enter()
{
    esp_err_t ret;

    /* Reset battery adc gpio */
    gpio_reset_pin(GPIO_NUM_36);
    /* Reset I2S gpio */
    gpio_reset_pin(GPIO_NUM_35);
    gpio_reset_pin(GPIO_NUM_25);
    gpio_reset_pin(GPIO_NUM_26);
    gpio_reset_pin(GPIO_NUM_23);
    gpio_reset_pin(GPIO_NUM_18);
    gpio_reset_pin(GPIO_NUM_5);
    gpio_reset_pin(GPIO_NUM_4);
    gpio_reset_pin(GPIO_NUM_0);
    /* Reset OLED gpio */
    gpio_reset_pin(GPIO_NUM_33);
    gpio_reset_pin(GPIO_NUM_32);
    gpio_reset_pin(GPIO_NUM_12);
    gpio_reset_pin(GPIO_NUM_13);
    gpio_reset_pin(GPIO_NUM_2);
    /* Reset encoder gpio */
    gpio_reset_pin(GPIO_NUM_27);
    gpio_reset_pin(GPIO_NUM_14);
    
    /* Reset codec power dc-dc */
    gpio_reset_pin(GPIO_NUM_22);
    /* Reset codec PA */
    gpio_reset_pin(GPIO_NUM_21);
    /* Reset OLED */
    gpio_reset_pin(GPIO_NUM_13);  

    /* Isolate battery adc gpio */
    // rtc_gpio_isolate(GPIO_NUM_36);
    // /* Isolate I2S gpio */
    // rtc_gpio_isolate(GPIO_NUM_35);
    // rtc_gpio_isolate(GPIO_NUM_25);
    // rtc_gpio_isolate(GPIO_NUM_26);
    // rtc_gpio_isolate(GPIO_NUM_22);
    // rtc_gpio_isolate(GPIO_NUM_23);
    // rtc_gpio_isolate(GPIO_NUM_18);
    // rtc_gpio_isolate(GPIO_NUM_5);
    // rtc_gpio_isolate(GPIO_NUM_4);
    // rtc_gpio_isolate(GPIO_NUM_0);
    // /* Isolate OLED gpio */
    // rtc_gpio_isolate(GPIO_NUM_32);
    // rtc_gpio_isolate(GPIO_NUM_33);
    // rtc_gpio_isolate(GPIO_NUM_12);
    // rtc_gpio_isolate(GPIO_NUM_2);
    // /* Isolate encoder gpio */
    // rtc_gpio_isolate(GPIO_NUM_27);
    // rtc_gpio_isolate(GPIO_NUM_14);
    
    /* Disable codec power dc-dc */
    // rtc_gpio_pulldown_en(GPIO_NUM_22);
    // /* Disable codec PA */
    // rtc_gpio_pulldown_en(GPIO_NUM_21);
    // /* Disable OLED */
    // rtc_gpio_pulldown_en(GPIO_NUM_13);    

    /* Set RTC IO as wakeup source */
    rtc_gpio_pullup_en(SLEEP_WAKEUP_PIN);
    ret = esp_sleep_enable_ext0_wakeup(SLEEP_WAKEUP_PIN, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set wakeup pin, %d", ret);
        return;
    }
    /* Start deep sleep */
    esp_deep_sleep_start();
}