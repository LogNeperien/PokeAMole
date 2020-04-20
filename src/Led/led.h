#ifndef __LED_INCLUDED__
#define __LED_INCLUDED__

#include "framework.h"

void led_init(uint8_t gpio_pin);
void led_set(uint8_t value);
void led_toggle();
void led_set_blink(uint32_t interval_ms, uint32_t duration_ms = 0);

#endif