#include <Arduino.h>
#include "esp_heap_caps.h"
#include "epd_driver.h"
#include "epd_highlevel.h"
#include "esp_adc_cal.h"
#include "esp_sleep.h"
#include "jbmono14.h"

#define log_info(format, ...) Serial.printf("\x1B[36m | " format "\033[0m\n", ##__VA_ARGS__)

gpio_num_t PIN_BATTERY = GPIO_NUM_36;
gpio_num_t PIN_BUTTON_1 = GPIO_NUM_0;
gpio_num_t PIN_BUTTON_2 = GPIO_NUM_35;
gpio_num_t PIN_BUTTON_3 = GPIO_NUM_34;
gpio_num_t PIN_BUTTON_4 = GPIO_NUM_39;

EpdiyHighlevelState epd_state;

uint32_t voltage_reference = 1100;
const char *message = "╭╸Wydarzenia╺─────────────┬╸Autobusy╺──────────────────╮\n"
                      "│ 08:15 - TKOM            │ 158 | 11:56, 12:26         │\n"
                      "│ 10:15 - WUS             │ 523 | 11:38, 11:56         │\n"
                      "│ 18:15 - PIS             │ 143 | 11:51, 12:01         │\n"
                      "│ 20:00 - Spotkanie       │ 188 | 11:40, 11:55         │\n"
                      "│                         ├╸Pogoda╺────────────────────┤\n"
                      "│                         │                            │\n"
                      "│                         │ 4°C, Pochmurnie            │\n"
                      "│                         │                            │\n"
                      "│                         │ 15:00 - 0°C, Śnieg         │\n"
                      "│                         │ Jutro - -4°C, Mgła         │\n"
                      "│                         │                            │\n"
                      "│                         │                            │\n"
                      "╰─────────────────────────┴───────────────────────╸45%╺╯";

void init_epd();

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
    EpdFontProperties font_props = epd_font_properties_default();
    font_props.flags = EPD_DRAW_ALIGN_LEFT;
    epd_write_string(&jbmono14, text, &cursor_x, &cursor_y, epd_state.front_fb, &font_props);

    epd_poweron();
    epd_hl_update_screen(&epd_state, MODE_GC16, 20);
    delay(500);
    epd_poweroff();
    delay(1000);
}

void start_deep_sleep() {
    epd_poweroff();
    delay(400);

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
    epd_init(EPD_OPTIONS_DEFAULT);
    epd_state = epd_hl_init(EPD_BUILTIN_WAVEFORM);
}

__attribute__((unused)) void setup() {
    Serial.begin(115200);
    init_adc();
    init_epd();

    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {
        log_info("woke up");
    } else if (wakeup_reason == ESP_SLEEP_WAKEUP_UNDEFINED) {
        log_info("first boot");
        epd_clear();
        display_message(message);

    } else {
        log_info("woke up by unknown reason: %u", wakeup_reason);
    }

    start_deep_sleep();
}

__attribute__((unused)) void loop() {

}