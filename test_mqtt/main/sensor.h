#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

bool sensor_init();

bool sensor_read(float &temperature,
                 float &humidity,
                 uint16_t &co2);

#endif