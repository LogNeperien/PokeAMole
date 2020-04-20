#ifndef __APP_INCLUDED__
#define __APP_INCLUDED__

#include "framework.h"
#include "Oled/oled.h"
#include "Led/led.h"
#include "Button/button.h"
#include "Mqtt/mqtt.h"
#include "Wifi/wifi.h"

void app_init();
void app_process(uint8_t gpio_pin);

#endif