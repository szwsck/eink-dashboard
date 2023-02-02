#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_adc_cal.h"
#include "esp_sleep.h"
#include "jbmono14.h"
#include "secrets.h"
#include "display.h"
#include "log.h"
#include "sleep.h"

gpio_num_t PIN_BATTERY = GPIO_NUM_36;

uint32_t voltage_reference = 1100;

double_t get_battery_percentage() {
    epd_poweron();
    delay(50);

    double_t voltage = ((double_t) analogRead(PIN_BATTERY) / 4095.0) * 2.0 * 3.3 * (voltage_reference / 1000.0);

    double_t percentage = ((voltage - 3.7) / 0.5) * 100;
    if (percentage > 100) percentage = 100;

    epd_poweroff();
    delay(50);

    return percentage;
}

void init_adc() {
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(
            ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars
    );
    assert(val_type == ESP_ADC_CAL_VAL_EFUSE_VREF);
    voltage_reference = adc_chars.vref;
}

void init_wifi() {
    log_info("connecting to %s", ssid);
    WiFi.begin(ssid, password);
    while (WiFiClass::status() != WL_CONNECTED) {
        delay(500);
        log_info(".");
    }
    log_info("connected to %s", ssid);
}

void download_message(char *buf) {
    log_info("downloading message");

    HTTPClient http;
    http.begin("http://51.83.140.52:10400/lilygo");

    int code = http.GET();

    if (code == 200) {
        String payload = http.getString();
        strcpy(buf, payload.c_str());
        log_info("downloaded message:\n%s", buf);
        return;
    } else {
        log_info("ERROR: invalid reponse code %u", code);
    }
}

RTC_DATA_ATTR uint32_t update_num;

void handle_first_boot() {
    log_info("handling first boot");
}

void update() {
    log_info("performing update %u", update_num);

    char text[1024];
    itoa((int) update_num, text, 10);

    display_init();
    display_show(text);
    display_deinit();

    update_num++;
}

__attribute__((unused)) void setup() {
    Serial.begin(115200);

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED) {
        handle_first_boot();
    }

    update();
}

__attribute__((unused)) void loop() {
    deep_sleep();
}