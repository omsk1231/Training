#include "wifi.h"

#include <Arduino.h>
#include <WiFi.h>

const char *SSID = "IBMS1";
const char *PASSWORD = "123456789";

bool wifi_init()
{
    Serial.println();
    Serial.println("=================================");
    Serial.println("Initializing WiFi");
    Serial.println("=================================");

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println();
    Serial.println("WiFi Connected Successfully");
    Serial.print("IP Address : ");
    Serial.println(WiFi.localIP());

    return true;
}