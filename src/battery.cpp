#include <Arduino.h>
#include <esp_adc_cal.h>
#include "epd_driver.h"
#include "log.h"
#include "battery.h"

#define BATT_PIN 36

uint32_t vref = 1100;

void correct_adc_reference() {
    log_info("correcting adc reference");
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(
            ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars
    );
    assert(val_type == ESP_ADC_CAL_VAL_EFUSE_VREF);
    vref = adc_chars.vref;
}

void get_battery_level(char *buf) {
    log_info("getting battery level");
    correct_adc_reference();

    display_init();
    epd_poweron();
    log_info("%.2f", epd_ambient_temperature());
    uint16_t reading = analogRead(BATT_PIN);
    epd_poweroff();

    double_t voltage = ((double_t) reading / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
    double_t charge_percent = ((voltage - 3.7) / 0.5) * 100;

    if (charge_percent > 100) {
        strcpy(buf, "USB");
    } else {
        sprintf(buf, "%2.f%%", charge_percent);
    }

}

double get_battery_voltage() {
    log_info("getting battery voltage");
    correct_adc_reference();

    display_init();
    epd_poweron();
    uint16_t reading = analogRead(BATT_PIN);
    epd_poweroff();

    return ((double_t) reading / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);

}