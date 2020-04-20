#ifndef __OLED_INCLUDED__
#define __OLED_INCLUDED__

#include "framework.h"

#include <U8g2lib.h>

void oled_init(uint8_t reset_gpio_pin, uint8_t scl_pin, uint8_t sda_pin);
void oled_print(String message);
void oled_display();
void oled_clear();

#endif