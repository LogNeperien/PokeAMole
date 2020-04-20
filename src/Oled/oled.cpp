#include "oled.h"

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

/* local types & variables */
U8G2_SSD1306_128X64_NONAME_F_HW_I2C *u8g2;

uint8_t cursor_y = 0;

/* private functions prototypes */

/* public functions */
void oled_init(uint8_t reset_gpio_pin, uint8_t scl_pin, uint8_t sda_pin)
{
    u8g2 = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, reset_gpio_pin, scl_pin, sda_pin);
    u8g2->begin();
    u8g2->setFont(u8g2_font_t0_11_mf);
    cursor_y = u8g2->getMaxCharHeight();
}

void oled_print(String message)
{
    uint8_t cursor_x = 0;
    uint8_t message_len = u8g2->getUTF8Width(message.c_str());

    if(OLED_WIDTH - message_len > 0)
    {
        cursor_x = (OLED_WIDTH - message_len)/2;
    }

    if(cursor_y > OLED_HEIGHT)
    {
        oled_clear();
    } 

    u8g2->drawUTF8(cursor_x, cursor_y, message.c_str());
    cursor_y += u8g2->getMaxCharHeight();   
}

void oled_display()
{
    u8g2->sendBuffer();
}

void oled_clear()
{
    cursor_y = u8g2->getMaxCharHeight();
    u8g2->clear();
}

/* private functions */