#include "display.h"
#include "epd_driver.h"
#include "epd_highlevel.h"
#include "log.h"
#include "jbmono14.h"

EpdiyHighlevelState state;

void display_init() {
    epd_init(EPD_OPTIONS_DEFAULT);
    state = epd_hl_init(EPD_BUILTIN_WAVEFORM);
}

void display_show(const char *text) {
    log_info("displaying text:\n%s\n", text);

    epd_hl_set_all_white(&state);

    int cursor_x = 4;
    int cursor_y = 30;
    epd_write_default(&jbmono14, text, &cursor_x, &cursor_y, state.front_fb);

    epd_poweron();
    epd_hl_update_screen(&state, MODE_GC16, 20);
    epd_poweroff();
}

void display_deinit() {
    epd_deinit();
}