#pragma once

#include "Arduino.h"
#include "display.h"
#include "sleep.h"

#define log_info(format, ...) Serial.printf("\x1B[36m | " format "\033[0m\n", ##__VA_ARGS__)

#define panic(format, ...) {\
    char error_message[512];\
    sprintf(error_message, format, ##__VA_ARGS__);\
    Serial.print("\x1B[31m");\
    Serial.println(error_message);\
    display_clear();\
    display_message(error_message);\
    deep_sleep();\
}
