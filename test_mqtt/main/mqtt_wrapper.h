#ifndef MQTT_WRAPPER_H
#define MQTT_WRAPPER_H

#include <Arduino.h>

bool mqtt_init();

void mqtt_publish(const String &payload);

#endif