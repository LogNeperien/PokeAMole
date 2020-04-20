#include "app.h"
#include "Button/button.h"
#include "Led/led.h"
#include "Oled/oled.h"

/* local types & variables */
enum AppStates {
    DEEP_SLEEP = 0,
    APP_NO_WIFI = 1,
    APP_NO_MQTT = 2,
    APP_NO_WIFI_NO_MQTT = 3,
    APP_READY = 4,
    APP_SIMPLE_CLIC = 5,
    LONG_PRESS = 6,
    APP_IDLE = 7
};

enum GameStates {
    NOT_STARTED,
    END,
    PLAY
};

uint8_t app_state;
uint8_t last_app_state;
uint8_t state_jeu;
uint32_t button_clic_time = 0;
uint32_t time_game = 0;
uint32_t time_reaction = 0;
int last_action = 0;
int now = 0;
uint8_t high_score = -1;
uint16_t timeToReset = 5 * 1000 + (5 * 1000 )/2;
uint16_t timeBegin;


/* private functions prototypes */

/* public functions */
void app_init()
{
    app_state = APP_READY;
    state_jeu = NOT_STARTED;
    timeBegin = millis();
}

void app_process(uint8_t gpio_pin)
{
    now = millis();

    if(app_state == APP_NO_WIFI || app_state == APP_NO_MQTT || app_state == APP_NO_WIFI_NO_MQTT) //deconnexion wifi
    {
        oled_clear();
        if(app_state == APP_NO_WIFI_NO_MQTT)
        {
            oled_print("Wifi Lost");
            oled_print("Server Lost");
            oled_print("Try to reconnect");
        }
        else if(app_state == APP_NO_WIFI)
        {
            oled_print("Wifi Lost");
            oled_print("Try to reconnect");
        }
        else
        {
            oled_print("Server Lost");
            oled_print("Try to reconnect");
        }
        oled_display();
    }
    else if(app_state == APP_READY && state_jeu == NOT_STARTED)
    {
        oled_clear();
        oled_print("Poke a mole");
        oled_print("Long Press to start");
        oled_display();
        led_set(1);
        last_app_state = app_state;
        app_state = APP_IDLE;
        last_action = millis();
        
    }
    else if(app_state == LONG_PRESS && state_jeu == NOT_STARTED && timeBegin + timeToReset < now)
    {
        oled_clear();
        oled_print("Game started");
        oled_display();
        uint32_t time_compteur = millis();
        led_set_blink(500, 5000);//Durant 5 secondes la LED clignote à 500ms d'intervalle
        while(time_compteur - now < 5000)
        {
            time_compteur = millis();
        }
        oled_clear();
        oled_display(); //on efface l'écran
        led_set(0); // on met eteind la led pour pouvoir la voir s'allumer après
        long rand = random(10001); //un chiffre aleatoire entre 0 et 10000ms
        time_compteur = millis();
        rand = rand + 3000;
        while(time_compteur - now < rand )
        {
            time_compteur = millis();
        }
        led_set(1); //on lance le jeuuuuuuu
        time_game = millis();
        last_app_state = app_state;
        app_state = APP_IDLE;
        state_jeu = PLAY;
        last_action = millis();
    }
    else if(app_state == APP_SIMPLE_CLIC && state_jeu == PLAY) // a finir
    {
        oled_clear();
        led_set(0);
        time_reaction = millis() - time_game;
        //2592
        if(time_reaction > 1000)
        {
            //tronquage
            int tempsMedium = time_reaction / 10;
            //259
            float tempsEnSeconde = (float) tempsMedium / 100;
            //2.59
            oled_print("Reaction time : " + String(tempsEnSeconde) + "s");
        }
        else
        {
            oled_print("Reaction time : " + String(time_reaction) + "ms");
        }
        oled_print("Press to restart");
        oled_display();

        String dateEtTime = getDate_cb();

        //envoyer le score sur aws
        envoie_score(time_reaction, dateEtTime);
        last_app_state = app_state;
        app_state = APP_IDLE;
        state_jeu = END;
        last_action = millis();
    }
    else if(app_state == APP_SIMPLE_CLIC && state_jeu == END)
    {
        last_app_state = app_state;
        app_state = APP_READY;
        state_jeu = NOT_STARTED;
        last_action = millis();
    }
    else if(app_state == APP_SIMPLE_CLIC && state_jeu == NOT_STARTED && high_score != -1) // a finir 
    {
        //on affiche le high score
        oled_clear();
        oled_print("High score");
        oled_print("time : " + String(getHighScore()));
        oled_print("user: " + getUserHighScore());
        oled_print("date: " + getDateHighScore());
        //afficher date + binome
        oled_display();
        uint32_t time_compteur = millis();
        while(time_compteur - now < 5000)
        {
            time_compteur = millis();
        }
        last_app_state = app_state;
        app_state = APP_READY;
        last_action = millis();
        
    }
    else if( now - last_action > 10*60*1000) //si pas d'action pendant 10 min
    {
        //deep sleep
        esp_sleep_enable_ext0_wakeup(gpio_num_t(gpio_pin), PRESS);
        esp_deep_sleep_start();
        last_app_state = app_state;
        app_state = APP_READY;
        state_jeu = NOT_STARTED;
        last_action = millis();
    }
    else if(app_state == LONG_PRESS && state_jeu == NOT_STARTED && timeBegin + timeToReset > now )
    {
        Serial.println("Reset Volatile");
        clearNonVolatile();
        last_app_state = app_state;
        app_state = APP_READY;
        last_action = millis();
    }
    
}

/* private functions */

void button_press_cb()
{
    button_clic_time = millis();
    app_state = APP_SIMPLE_CLIC;
}


void button_long_press_cb() //si on reste appuyé sur le bouton longtemps
{
    app_state = LONG_PRESS;
}

void wifi_disconnected_cb()
{
    app_state = APP_NO_WIFI;
}

void wifi_connected_cb()
{
    //Serial.println("wifi connected");
    if(app_state == APP_NO_WIFI || app_state == APP_NO_WIFI_NO_MQTT)
    {
        Serial.println(app_state);

        if(app_state == APP_NO_WIFI)
            app_state = last_app_state; 
        else 
            app_state = APP_NO_MQTT;  
    }
}

void mqtt_disconnected_cb()
{
    if(app_state != APP_NO_WIFI)
    {
        app_state = APP_NO_MQTT;
    }
    else
    {
        app_state = APP_NO_WIFI_NO_MQTT;
    }
       
}

void mqtt_connected_cb()
{
    //Serial.println("mqtt connected");
    if(app_state == APP_NO_MQTT)
    {
        app_state = last_app_state;
        
    }
}