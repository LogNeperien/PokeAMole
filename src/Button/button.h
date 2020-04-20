#ifndef __BUTTON_INCLUDED__
#define __BUTTON_INCLUDED__

#include "framework.h"

#define PRESS 0
#define RELEASE 1
#define MULTIPLE_CLICK_MAX_NUMBER 2

#define DEBOUNCE_INTERVAL 20
#define MULTIPLE_CLICK_INTERVAL 300
#define LONG_PRESS_DURATION 5000

void button_init(uint8_t gpio);
void button_process();

void button_press_cb();
void button_long_press_cb();
void button_released_cb(uint8_t click_count);

#endif