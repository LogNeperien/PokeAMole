#ifndef __WIFI_INCLUDED__
#define __WIFI_INCLUDED__

#include "framework.h"
#include "Oled/oled.h"
#include "Mqtt/mqtt.h"

void wifi_init(String captive_portal_ssid);
void wifi_start_captive_portal();
void wifi_process();
void wifi_connected_cb();
void wifi_disconnected_cb();
String getDate_cb();

#endif