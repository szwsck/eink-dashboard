#include <esp_sleep.h>
#include "sleep.h"
#include "log.h"

gpio_num_t PIN_BUTTON_4 = GPIO_NUM_39;

void deep_sleep() {
    log_info("starting sleep");
    esp_sleep_enable_ext0_wakeup(PIN_BUTTON_4, 0);
    esp_deep_sleep_start();
}