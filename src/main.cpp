#include "aws.h"
#include "conf.h"

#include "App/app.h"
#include "Button/button.h"
#include "Led/led.h"
#include "Mqtt/mqtt.h"
#include "Oled/oled.h"
#include "Wifi/wifi.h"

/* local types & variables */

/* private functions prototypes */

/* public functions */

/* private functions */
void setup(void)
{
    Serial.begin(115200);
    
    button_init(BUTTON_GPIO);
    led_init(LED_GPIO);
    oled_init(SSD1306_RESET_GPIO, SSD1306_SCL_GPIO, SSD1306_SDA_GPIO);

    wifi_init("PokeAMole-" + String(TEAM_NAME));
    mqtt_init(AWS_END_POINT, AWS_CERT_CA, AWS_CERT_CRT, AWS_CERT_PRIVATE, DEVICE_NAME);
    app_init();
}

void loop(void)
{
    button_process();
    app_process(BUTTON_GPIO);
    wifi_process();
    mqtt_process(AWS_END_POINT, AWS_CERT_CA, AWS_CERT_CRT, AWS_CERT_PRIVATE, DEVICE_NAME);
}