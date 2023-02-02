#pragma once
#include "Arduino.h"

#define log_info(format, ...) Serial.printf("\x1B[36m | " format "\033[0m\n", ##__VA_ARGS__)
