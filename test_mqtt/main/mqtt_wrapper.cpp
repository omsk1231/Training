#include "mqtt_wrapper.h"

extern "C"
{
#include "mqtt_client.h"
}

#include <Arduino.h>

static esp_mqtt_client_handle_t mqtt_client = NULL;

#define MQTT_BROKER_URI "mqtt://10.129.2.123:1883"
#define MQTT_USERNAME   "admin"
#define MQTT_PASSWORD   "admin123"

#define MQTT_TOPIC      "test/CO2/om"

static void mqtt_event_handler(void *handler_args,
                               esp_event_base_t base,
                               int32_t event_id,
                               void *event_data)
{
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        Serial.println("MQTT Connected");
        break;

    case MQTT_EVENT_DISCONNECTED:
        Serial.println("MQTT Disconnected");
        break;

    default:
        break;
    }
}

bool mqtt_init()
{
    Serial.println();
    Serial.println("=================================");
    Serial.println("Initializing MQTT");
    Serial.println("=================================");

    esp_mqtt_client_config_t mqtt_cfg = {};

    mqtt_cfg.broker.address.uri = MQTT_BROKER_URI;
    mqtt_cfg.credentials.username = MQTT_USERNAME;
    mqtt_cfg.credentials.authentication.password = MQTT_PASSWORD;

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);

    if (mqtt_client == NULL)
    {
        Serial.println("Failed to create MQTT Client");
        return false;
    }

    esp_mqtt_client_register_event(
        mqtt_client,
        MQTT_EVENT_ANY,
        mqtt_event_handler,
        NULL);

    esp_mqtt_client_start(mqtt_client);

    delay(2000);

    return true;
}

void mqtt_publish(const String &payload)
{
    if (mqtt_client == NULL)
        return;

    int msg_id = esp_mqtt_client_publish(
        mqtt_client,
        MQTT_TOPIC,
        payload.c_str(),
        0,
        1,
        0);

    if (msg_id >= 0)
    {
        Serial.println();
        Serial.println("Published Successfully");
        Serial.println(payload);
    }
    else
    {
        Serial.println("Publish Failed");
    }
}