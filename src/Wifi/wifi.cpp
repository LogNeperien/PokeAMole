#include "wifi.h"

#include <ESP_WiFiManager.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

/* local types & variables */
ESP_WiFiManager wifi_manager;
String  portal_ssid;

/* private functions prototypes */
String formattageDate (String dayStamp);

/* public functions */
void wifi_init(String captive_portal_ssid)
{
    String router_ssid = wifi_manager.getStoredWiFiSSID();
    String router_pwd = wifi_manager.getStoredWiFiPass();
    portal_ssid = captive_portal_ssid;

    if(router_ssid == "")
    {
        router_ssid = getWifiSSID();
        router_pwd = getWifiPassword();
    }

    Serial.println("Wifi avant portail : " + router_ssid);

    if (router_ssid != "")
    {
        oled_clear();
        oled_print("waiting for wifi");
        oled_print(router_ssid);
        oled_display();

        WiFi.mode(WIFI_STA);
        WiFi.persistent(true);

        WiFi.begin(router_ssid.c_str(), router_pwd.c_str());
        while (WiFi.waitForConnectResult() != WL_CONNECTED)
        {
            Serial.print(".");
            delay(500);
        }
    }
    else
    {
        oled_clear();
        oled_print("Please setup");
        oled_print("wifi");
        oled_display();

        wifi_start_captive_portal();

    }

    Serial.print("Connecting to ");
    Serial.println("IP " + WiFi.localIP().toString());
}

void wifi_start_captive_portal()
{
    wifi_manager.startConfigPortal(portal_ssid.c_str());

    Serial.println(wifi_manager.getStoredWiFiSSID());
    Serial.println(wifi_manager.getStoredWiFiPass());
    setWifi(wifi_manager.getStoredWiFiSSID() , wifi_manager.getStoredWiFiPass());
}

void wifi_process()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        bool affichage = false;
        if(affichage == false)
        {
            wifi_disconnected_cb();
            affichage = true;
        }   
    }
    else
    {
        wifi_connected_cb();
    }
}


__attribute__((weak)) void wifi_connected_cb()
{
    // To implemented by user somewhere in the application 
}

__attribute__((weak)) void wifi_disconnected_cb()
{
    // To implemented by user somewhere in the application 
}


//source : https://randomnerdtutorials.com/esp32-ntp-client-date-time-arduino-ide/?fbclid=IwAR1fwBGgFswQy3D6IpoH6IDhPRmNQu6UOap3LDuKi-ZZM_2U-bvF8Wrt6ak
String getDate_cb()
{
    String finalString = "";
    WiFiUDP ntpUDP;
    NTPClient timeClient(ntpUDP);

    String formattedDate;
    String dayStamp;
    String timeStamp;

    //Serial.begin(115200);

    timeClient.begin();
    timeClient.setTimeOffset(3600*2);
    while(!timeClient.update()) {
        timeClient.forceUpdate();
    }
    formattedDate = timeClient.getFormattedDate();

    // Extract date
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    Serial.println(dayStamp);
    // Extract time
    timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
    Serial.println(timeStamp);

    String finalDate = formattageDate(dayStamp);

    finalString = finalDate + " " + timeStamp;
    return finalString;
}

/* private functions */
String formattageDate (String dayStamp)
{
    String day;
    String month;
    String year;

    int splitY = dayStamp.indexOf("-");
    year = dayStamp.substring(2, splitY);

    month = dayStamp.substring(5, 7);

    day = dayStamp.substring(8, 10);

    String finalDate = day + "/" + month + "/" + year;
    Serial.println(" finalDate = " + finalDate);
    return finalDate;
}