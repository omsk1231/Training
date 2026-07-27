#include "sensor.h"

#include <Arduino.h>

extern "C"
{
#include "i2cdev.h"
#include "scd4x.h"
}

#define SDA_PIN GPIO_NUM_21
#define SCL_PIN GPIO_NUM_22

static i2c_dev_t scd4x;

bool sensor_init()
{
    Serial.println();
    Serial.println("=================================");
    Serial.println("Initializing SCD40");
    Serial.println("=================================");

    i2cdev_init();

    esp_err_t err = scd4x_init_desc(
        &scd4x,
        I2C_NUM_0,
        SDA_PIN,
        SCL_PIN);

    if (err != ESP_OK)
    {
        Serial.printf("SCD40 Initialization Failed : %d\n", err);
        return false;
    }

    scd4x_stop_periodic_measurement(&scd4x);

    delay(500);

    err = scd4x_start_periodic_measurement(&scd4x);

    if (err != ESP_OK)
    {
        Serial.printf("Measurement Start Failed : %d\n", err);
        return false;
    }

    Serial.println("SCD40 Initialized Successfully");

    return true;
}

bool sensor_read(float &temperature,
                 float &humidity,
                 uint16_t &co2)
{
    bool ready = false;

    if (scd4x_get_data_ready_status(&scd4x, &ready) != ESP_OK)
        return false;

    if (!ready)
        return false;

    if (scd4x_read_measurement(
            &scd4x,
            &co2,
            &temperature,
            &humidity) != ESP_OK)
        return false;

    return true;
}