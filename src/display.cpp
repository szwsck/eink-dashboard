#include "display.h"
#include "epd_driver.h"
#include "epd_highlevel.h"
#include "log.h"
#include "jbmono14.h"

EpdiyHighlevelState state;
bool display_initialized = false;

void display_init() {
    if (display_initialized) return;
    epd_init(EPD_OPTIONS_DEFAULT);
    state = epd_hl_init(EPD_BUILTIN_WAVEFORM);
    display_initialized = true;
}

void display_clear() {
    if (!display_initialized) display_init();
    log_info("clearing display");
    epd_clear();
}

void display_message(const char *text, uint8_t offset_x, uint8_t offset_y) {
    if (!display_initialized) display_init();
    log_info("displaying text:\n%s", text);

    int cursor_x = 4 + offset_x * jbmono14_Glyphs[0].advance_x;
    int cursor_y = 25 + offset_y * jbmono14.advance_y;
    epd_write_default(&jbmono14, text, &cursor_x, &cursor_y, state.front_fb);

    epd_poweron();
    epd_hl_update_screen(&state, MODE_GC16, epd_ambient_temperature());
    epd_poweroff();
}

void display_deinit() {
    if (!display_initialized) return;
    epd_deinit();
    display_initialized = false;
}