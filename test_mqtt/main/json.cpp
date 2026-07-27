#include "json.h"

String create_sensor_json(float temperature,
                          float humidity,
                          uint16_t co2)
{
    String payload = "{";

    payload += "\"co2\":";
    payload += String(co2);
    payload += ",";

    payload += "\"temperature\":";
    payload += String(temperature, 2);
    payload += ",";

    payload += "\"humidity\":";
    payload += String(humidity, 2);

    payload += "}";

    return payload;
}