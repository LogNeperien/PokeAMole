#include "led.h"
#include <Ticker.h>

/* local types & variables */
uint8_t led_gpio;
uint8_t led_blink = 0;
Ticker led_interval_timer;
Ticker led_duration_timer;

/* private functions prototypes */
void led_toggle_blink();
void led_stop_blink();

/* public functions */
void led_init(uint8_t gpio_pin)
{
    led_gpio = gpio_pin;
    pinMode(led_gpio, OUTPUT);
    digitalWrite(led_gpio, 0);
}

void led_set(uint8_t value)
{
    if (led_blink == 1) {
        led_stop_blink();
        led_blink = 0;
    }

    digitalWrite(led_gpio, value);
}

void led_toggle()
{
    uint8_t value = digitalRead(led_gpio);

    if (led_blink == 1) {
        led_stop_blink();
        led_blink = 0;
    }

    value ^= 1;
    digitalWrite(led_gpio, value);
}

void led_set_blink(uint32_t interval_ms, uint32_t duration_ms)
{
    led_blink = 1;
    led_interval_timer.attach_ms(interval_ms, led_toggle_blink);

    if (duration_ms != 0)
    {
        led_duration_timer.once_ms(duration_ms, led_stop_blink);
    }
}

/* private functions */
void led_toggle_blink()
{
    uint8_t value = digitalRead(led_gpio);

    value ^= 1;
    digitalWrite(led_gpio, value);
}

void led_stop_blink()
{
    led_blink = 0;
    led_interval_timer.detach();
    led_duration_timer.detach();
}