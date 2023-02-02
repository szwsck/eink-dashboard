#include <esp_sleep.h>
#include "sleep.h"
#include "log.h"
#include "display.h"

#define SLEEP_US (10 * 60 * 1000 * 1000)

gpio_num_t PIN_BUTTON_4 = GPIO_NUM_39;

void deep_sleep() {
    display_deinit();
    log_info("starting sleep");
    esp_sleep_enable_ext0_wakeup(PIN_BUTTON_4, 0);
    esp_sleep_enable_timer_wakeup(SLEEP_US);
    esp_deep_sleep_start();
}