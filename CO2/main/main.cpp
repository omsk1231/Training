#include <Arduino.h>

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2cdev.h"
#include "scd4x.h"
}

#define SDA_PIN GPIO_NUM_21
#define SCL_PIN GPIO_NUM_22

i2c_dev_t scd;

void setup()
{
    Serial.begin(115200);
    delay(2000);

    Serial.println();
    Serial.println("================================");
    Serial.println("SCD41 CO2 Sensor Example");
    Serial.println("================================");

    ESP_ERROR_CHECK(i2cdev_init());

    ESP_ERROR_CHECK(
        scd4x_init_desc(
            &scd,
            I2C_NUM_0,
            SDA_PIN,
            SCL_PIN));

    Serial.println("Sensor detected");

    ESP_ERROR_CHECK(scd4x_wake_up(&scd));
    ESP_ERROR_CHECK(scd4x_stop_periodic_measurement(&scd));
    ESP_ERROR_CHECK(scd4x_reinit(&scd));

    uint16_t serial0, serial1, serial2;

    ESP_ERROR_CHECK(
        scd4x_get_serial_number(
            &scd,
            &serial0,
            &serial1,
            &serial2));

    Serial.printf("Serial Number : %04X%04X%04X\n",
                  serial0,
                  serial1,
                  serial2);

    ESP_ERROR_CHECK(
        scd4x_start_periodic_measurement(&scd));

    Serial.println("Periodic measurement started");
    Serial.println();
}

void loop()
{
    uint16_t co2;
    float temperature;
    float humidity;

    esp_err_t err =
        scd4x_read_measurement(
            &scd,
            &co2,
            &temperature,
            &humidity);

    if (err == ESP_OK && co2 != 0)
    {
        Serial.println("--------------------------------");
        Serial.printf("CO2         : %u ppm\n", co2);
        Serial.printf("Temperature : %.2f C\n", temperature);
        Serial.printf("Humidity    : %.2f %%\n", humidity);
    }
    else
    {
        Serial.println("Waiting for first measurement...");
    }

    delay(5000);
}

extern "C" void app_main(void)
{
    initArduino();

    setup();

    while (true)
    {
        loop();
    }
}