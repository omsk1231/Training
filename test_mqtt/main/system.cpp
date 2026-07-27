#include "system.h"

#include "wifi.h"
#include "sensor.h"
#include "mqtt_wrapper.h"
#include "json.h"

bool system_init()
{
    if (!wifi_init())
        return false;

    if (!sensor_init())
        return false;

    if (!mqtt_init())
        return false;

    Serial.println();
    Serial.println("=================================");
    Serial.println("System Ready");
    Serial.println("=================================");

    return true;
}

void system_run()
{
    float temperature;
    float humidity;
    uint16_t co2;

    if (!sensor_read(temperature, humidity, co2))
        return;

    String payload = create_sensor_json(
        temperature,
        humidity,
        co2);

    mqtt_publish(payload);
}