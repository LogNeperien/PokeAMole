#include "button.h"

/* local types & variables */
enum ButtonStates
{
    RELEASED = 0,
    PRESSED,
    LONG_PRESSED
};

typedef struct Button
{
    uint8_t gpio_pin;
    ButtonStates state = RELEASED;
    uint32_t state_time = 0;
    uint8_t click_count = 0;
} button_t;

button_t *button;

/* private functions prototypes */

/* public functions */
void button_init(uint8_t gpio_pin)
{
    button = new button_t;
    button->gpio_pin = gpio_pin;
    pinMode(button->gpio_pin, INPUT);
}

void button_process()
{
    uint8_t button_value = digitalRead(button->gpio_pin);
    uint32_t now = millis();

    switch (button->state)
    {
        case PRESSED:
            if (button_value == PRESS)
            {
                if (now - button->state_time > LONG_PRESS_DURATION)
                {
                    button_long_press_cb();
                    button->state = LONG_PRESSED;
                    button->state_time = now;
                }
            }
            else if (now - button->state_time > DEBOUNCE_INTERVAL)
            {
                button_released_cb(button->click_count);
                button->state = RELEASED;
                button->state_time = now;
            }
            break;

        case LONG_PRESSED:
            if (button_value == RELEASE) {
                button_released_cb(button->click_count);
                button->state = RELEASED;
                button->state_time = now;
                button->click_count = 0;
            }
            break;

        case RELEASED:
        default:
            if (button_value == PRESS && (now - button->state_time > DEBOUNCE_INTERVAL))
            {
                button->click_count++;
                button_press_cb();
                button->state = PRESSED;
                button->state_time = now;
            }
            else if (now - button->state_time > MULTIPLE_CLICK_INTERVAL)
            {
                button->click_count = 0;
            }
            break;
    }
}

__attribute__((weak)) void button_press_cb()
{
    // To implemented by user somewhere in the application code
}

__attribute__((weak)) void button_long_press_cb()
{
    // To implemented by user somewhere in the application 
}

__attribute__((weak)) void button_released_cb(uint8_t click_count)
{
    // To implemented by user somewhere in the application code
}

/* private functions */