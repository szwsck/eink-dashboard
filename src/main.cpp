#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "epd_driver.h"
#include "epd_highlevel.h"
#include "esp_adc_cal.h"
#include "esp_sleep.h"
#include "jbmono14.h"
#include "secrets.h"

#define log_info(format, ...) Serial.printf("\x1B[36m | " format "\033[0m\n", ##__VA_ARGS__)

gpio_num_t PIN_BATTERY = GPIO_NUM_36;
gpio_num_t PIN_BUTTON_1 = GPIO_NUM_0;
gpio_num_t PIN_BUTTON_2 = GPIO_NUM_35;
gpio_num_t PIN_BUTTON_3 = GPIO_NUM_34;
gpio_num_t PIN_BUTTON_4 = GPIO_NUM_39;

EpdiyHighlevelState epd_state;

uint32_t voltage_reference = 1100;

void log_wakeup_cause();

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

void display_message(const char *text) {
    epd_hl_set_all_white(&epd_state);

    int cursor_x = 4;
    int cursor_y = 30;
    epd_write_default(&jbmono14, text, &cursor_x, &cursor_y, epd_state.front_fb);

    epd_poweron();
    epd_hl_update_screen(&epd_state, MODE_GC16, 20);
    epd_poweroff();
}

void start_deep_sleep() {
    epd_poweroff();
    epd_deinit();
    esp_sleep_enable_ext0_wakeup(PIN_BUTTON_1, 0);
    log_info("starting sleep");
    esp_deep_sleep_start();
}

void init_adc() {
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(
            ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars
    );
    assert(val_type == ESP_ADC_CAL_VAL_EFUSE_VREF);
    voltage_reference = adc_chars.vref;
}

void init_epd() {
    epd_init(EPD_LUT_1K);
    epd_state = epd_hl_init(EPD_BUILTIN_WAVEFORM);
    epd_clear();
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

void log_wakeup_cause() {
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause == ESP_SLEEP_WAKEUP_EXT0) {
        log_info("woke up");
    } else if (cause == ESP_SLEEP_WAKEUP_UNDEFINED) {
        log_info("first boot");
    } else {
        log_info("woke up by unknown cause: %u", cause);
    }
}

__attribute__((unused)) void setup() {
    Serial.begin(115200);

    log_wakeup_cause();

//    init_adc();
    init_epd();
    init_wifi();
}

__attribute__((unused)) void loop() {
    char buf[2048] = "hello!";
//    download_message(buf);
    display_message(buf);

    start_deep_sleep();
}