#include <WiFi.h>
#include <HTTPClient.h>
#include "net.h"
#include "log.h"
#include "secrets.h"
#include "battery.h"

#define WIFI_TIMEOUT 20
#define HTTP_TIMEOUT 20

bool wifi_initialized = false;

void wifi_init() {
    if (wifi_initialized) return;
    log_info("connecting to %s", ssid);

    WiFi.begin(ssid, password);
    delay(500);
    int tries = 0;
    while (WiFiClass::status() != WL_CONNECTED) {
        log_info(".");
        delay(1000);
        if (tries++ > WIFI_TIMEOUT) {
            panic("couldn't connect to %s", ssid);
        }
    }
    log_info("connected to %s", ssid);
}

bool update_message(char *etag, char *buf) {
    if (!wifi_initialized) wifi_init();
    log_info("downloading message (current is %s)", etag);

    HTTPClient http;
    http.setTimeout(HTTP_TIMEOUT * 1000);
    http.setConnectTimeout(HTTP_TIMEOUT * 1000);
    http.begin(String(url) + "?voltage=" + String(get_battery_voltage()));
    http.addHeader("If-None-Match", String(etag));
    const char *responseHeaders[] = {"ETag"};
    http.collectHeaders(responseHeaders, 1);
    int code = http.GET();

    if (code == 304) {
        log_info("no need to update message with etag %s", etag);
        return false;
    }

    if (code == 200) {
        // TODO this probably could be done without double allocation
        strcpy(etag, http.header("ETag").c_str());
        strcpy(buf, http.getString().c_str());
        log_info("got new message with etag %s", etag);
        return true;
    }

    panic("invalid reponse code %u", code)
    return true;

}