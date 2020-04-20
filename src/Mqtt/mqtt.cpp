#include "mqtt.h"

#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

/* local types & variables */
enum MqttStates
{
    MQTT_IDLE
};

WiFiClientSecure secureClient = WiFiClientSecure();
PubSubClient mqttClient(secureClient);
MqttStates mqtt_state;

Preferences preferences;

/* private functions prototypes */

/* public functions */
void mqtt_init(const char *end_point, const char *cert_ca, const char *cert_crt, const char *cert_private, const char *device_name)
{
    oled_clear();
    oled_print("Waiting for server");
    oled_display();

    mqttClient.setServer(end_point, 8883);
    secureClient.setCACert(cert_ca);
    secureClient.setCertificate(cert_crt);
    secureClient.setPrivateKey(cert_private);

    Serial.println("Connecting to MQTT....");
    mqttClient.connect(device_name);

    while (!mqttClient.connected())
    {
      Serial.println("Connecting to MQTT....Retry");
      mqttClient.connect(device_name);
      delay(5000);
    }


    Serial.println("MQTT Connected");

    mqttClient.subscribe("ecepp/PokeAMole/score"); //si on veut changer pour lire c'est ici
    mqttClient.setCallback(mqtt_inbox);

}

void mqtt_process(const char *end_point, const char *cert_ca, const char *cert_crt, const char *cert_private, const char *device_name)
{
    if (!mqttClient.connected())
    {
      mqtt_disconnected_cb();
      Serial.println("Connecting to MQTT....Retry");
      mqttClient.connect(device_name);
      delay(5000);
    }
    else
    {
      mqtt_connected_cb();
    }
    //ici callback pour la ligne suivante
    mqttClient.loop();

}

/* private functions */
void mqtt_inbox(String topic, byte* payload, unsigned int length)
{
  preferences.begin("my-app", false);
  if (length > 0) {
    char* message = new char[length];
    

    unsigned int noVolatileHighScore = preferences.getUInt("HighScore", 0);
    //void * memcpy ( void * destination, const void * source, size_t num );
    // destination = Pointer to the destination array where the content is to be copied, type-casted to a pointer of type void*.
    // source = Pointer to the source of data to be copied, type-casted to a pointer of type const void*.
    //length = Number of bytes to copy.
    memcpy(message, payload, length);
    message[length] = 0;

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, message);

    String scoreString = doc["score"];
    String user = doc["user"];
    String date = doc["date"];
    Serial.println(scoreString);
    Serial.println(user);
    Serial.println(date);

    uint32_t score = scoreString.toInt();
    
    
    Serial.println("Message received from topic " + topic + " payload " + message);
    Serial.println(score);

    if((score < noVolatileHighScore || noVolatileHighScore == 0) && ((date != NULL && user != NULL) && (date != "null" && user != "null")))
    {
      preferences.putUInt("HighScore", score);
      preferences.putString("user", user);
      preferences.putString("date", date);

      Serial.println("J'ai changé le high score : ");
      noVolatileHighScore = preferences.getUInt("HighScore", 0);
      Serial.println(String(noVolatileHighScore));
    }
  }
  preferences.end();
}

void envoie_score(uint32_t score, String date)
{
    String user = "B02";
    String scoreString;
    scoreString = String(score);

    Serial.print("user : ");
    Serial.println(user);
    Serial.print("score : ");
    Serial.println(scoreString);
    Serial.print("date : ");
    Serial.println(date);

    StaticJsonDocument<128> jsonDoc;
    JsonObject eventDoc = jsonDoc.createNestedObject("event");
    eventDoc["user"] = user;
    eventDoc["score"] = score;
    eventDoc["date"] = date;

    char jsonBuffer[128];

    serializeJson(eventDoc, jsonBuffer);
    mqttClient.publish("ecepp/PokeAMole/score", jsonBuffer);
}

uint32_t getHighScore()
{
  uint32_t highScore;
  preferences.begin("my-app", false);
  highScore = preferences.getUInt("HighScore", 0);
  preferences.end();
  return highScore;
}

String getUserHighScore()
{
  String user;
  preferences.begin("my-app", false);
  user = preferences.getString("user", "");
  preferences.end();
  return user;
}

String getDateHighScore()
{
  String date;
  preferences.begin("my-app", false);
  date = preferences.getString("date", "");
  preferences.end();
  return date;
}

void setWifi(String ssid, String mdp)
{
  preferences.begin("my-app", false);
  preferences.putString("ssid", ssid);
  preferences.putString("pswd", mdp);
  preferences.end();
}

String getWifiSSID()
{
  String ssid;
  preferences.begin("my-app", false);
  ssid = preferences.getString("ssid", "");
  preferences.end();
  return ssid;
}

String getWifiPassword()
{
  String pswd;
  preferences.begin("my-app", false);
  pswd = preferences.getString("pswd", "");
  preferences.end();
  return pswd;
}

void clearNonVolatile()
{
  preferences.begin("my-app", false);
  preferences.clear();
  preferences.end();
}

//weak function 
__attribute__((weak)) void mqtt_connected_cb()
{
    // To implemented by user somewhere in the application 
}

__attribute__((weak)) void mqtt_disconnected_cb()
{
    // To implemented by user somewhere in the application 
}