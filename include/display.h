#pragma once

#include <cstdint>

void display_init();

void display_clear();

void display_message(const char *text, uint8_t offset_x = 0, uint8_t offset_y = 0);

void display_deinit();