#ifndef JSON_H
#define JSON_H

#include <Arduino.h>

String create_sensor_json(float temperature,
                          float humidity,
                          uint16_t co2);

#endif