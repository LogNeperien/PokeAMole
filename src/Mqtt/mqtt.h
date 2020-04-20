#ifndef __MQTT_INCLUDED__
#define __MQTT_INCLUDED__

#include "framework.h"
#include "Oled/oled.h"
#include <Preferences.h>

void mqtt_init(const char *end_point, const char *cert_ca, const char *cert_crt, const char *cert_private, const char *device_name);
void mqtt_process(const char *end_point, const char *cert_ca, const char *cert_crt, const char *cert_private, const char *device_name);

void mqtt_inbox(String topic, byte* payload, unsigned int length);
void envoie_score(uint32_t score, String date);
uint32_t getHighScore();
String getUserHighScore();
String getDateHighScore();
void setWifi(String ssid, String mdp);
String getWifiSSID();
String getWifiPassword();
void clearNonVolatile();

void mqtt_connected_cb();
void mqtt_disconnected_cb();

#endif