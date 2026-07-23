#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"
#include "i2c_bus.h"
#include "aht20.h"

#define I2C_MASTER_NUM      I2C_NUM_0
#define I2C_MASTER_SDA_IO   GPIO_NUM_21
#define I2C_MASTER_SCL_IO   GPIO_NUM_22
#define I2C_MASTER_FREQ_HZ  100000

extern "C" void app_main(void)
{
    // Configure I2C
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {
            .clk_speed = I2C_MASTER_FREQ_HZ,
        },
    };

    // Create I2C Bus
    i2c_bus_handle_t bus = i2c_bus_create(I2C_MASTER_NUM, &conf);

    // Configure AHT20
    aht20_i2c_config_t aht_cfg = {
        .bus_inst = bus,
        .i2c_addr = AHT20_ADDRRES_0,
    };

    aht20_dev_handle_t sensor = NULL;

    ESP_ERROR_CHECK(aht20_new_sensor(&aht_cfg, &sensor));

    while (1)
    {
        uint32_t temp_raw = 0;
        uint32_t hum_raw = 0;

        float temperature = 0.0f;
        float humidity = 0.0f;

        esp_err_t ret = aht20_read_temperature_humidity(
                            sensor,
                            &temp_raw,
                            &temperature,
                            &hum_raw,
                            &humidity);

        if (ret == ESP_OK)
        {
            printf("---------------------------\n");
            printf("Temperature : %.2f C\n", temperature);
            printf("Humidity    : %.2f %%\n", humidity);
            printf("Raw Temp    : %lu\n", (unsigned long)temp_raw);
            printf("Raw Hum     : %lu\n", (unsigned long)hum_raw);
        }
        else
        {
            printf("Sensor Read Failed! Error = %d\n", ret);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}