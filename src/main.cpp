#include <Arduino.h>
#include "display.h"
#include "log.h"
#include "sleep.h"
#include "net.h"
#include "battery.h"

RTC_DATA_ATTR char message_etag[64] = "initial";
char message[2048];

void update(bool first_boot) {
    log_info("performing update");

    if (first_boot) display_clear();

    bool updated = update_message(message_etag, message);
    if (updated) display_message(message);

    char battery_level[4];
    get_battery_level(battery_level);
    display_message(battery_level, 37, 10);
}

__attribute__((unused)) void setup() {
    Serial.begin(115200);

    esp_sleep_wakeup_cause_t wakeup_cause = esp_sleep_get_wakeup_cause();

    update(wakeup_cause == ESP_SLEEP_WAKEUP_UNDEFINED);
}

__attribute__((unused)) void loop() {
    deep_sleep();
}