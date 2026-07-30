#include "CO2.h"

#include <stdint.h>

#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


static const char *TAG = "CO2_SENSOR";


/*
 * Keep the latest successfully received CO2 value.
 *
 * If the sensor does not provide a new valid reading,
 * the dashboard continues displaying this value instead
 * of temporarily displaying 0 ppm.
 */
static float last_valid_co2 = 0.0f;


/* =========================================================
 * I2C CONFIGURATION
 * ========================================================= */

#define I2C_PORT            I2C_NUM_0

#define I2C_SDA_PIN         GPIO_NUM_21
#define I2C_SCL_PIN         GPIO_NUM_22

#define I2C_FREQUENCY       100000

#define SCD41_ADDRESS       0x62


/* =========================================================
 * SCD41 COMMANDS
 * ========================================================= */

#define SCD41_START_PERIODIC_MSB       0x21
#define SCD41_START_PERIODIC_LSB       0xB1

#define SCD41_READ_MEASUREMENT_MSB     0xEC
#define SCD41_READ_MEASUREMENT_LSB     0x05


/* =========================================================
 * CRC CALCULATION
 * ========================================================= */

/**
 * @brief Calculate Sensirion CRC-8.
 *
 * SCD41 sends a CRC byte after every two data bytes.
 *
 * Polynomial: 0x31
 * Initial value: 0xFF
 */
static uint8_t calculate_crc(
    const uint8_t *data,
    uint8_t length)
{
    uint8_t crc = 0xFF;


    for (uint8_t i = 0; i < length; i++)
    {
        crc ^= data[i];


        for (uint8_t bit = 0; bit < 8; bit++)
        {
            if (crc & 0x80)
            {
                crc =
                    (crc << 1) ^ 0x31;
            }
            else
            {
                crc <<= 1;
            }
        }
    }


    return crc;
}


/* =========================================================
 * SEND SCD41 COMMAND
 * ========================================================= */

/**
 * @brief Send a two-byte command to the SCD41.
 */
static esp_err_t send_command(
    uint8_t command_msb,
    uint8_t command_lsb)
{
    uint8_t command[2] =
    {
        command_msb,
        command_lsb
    };


    return i2c_master_write_to_device(
        I2C_PORT,
        SCD41_ADDRESS,
        command,
        sizeof(command),
        pdMS_TO_TICKS(1000)
    );
}


/* =========================================================
 * INITIALIZE CO2 SENSOR
 * ========================================================= */

esp_err_t co2_init(void)
{
    ESP_LOGI(
        TAG,
        "Initializing SCD41..."
    );


    /* =====================================================
     * CONFIGURE I2C MASTER
     * ===================================================== */

    i2c_config_t i2c_config = {};


    i2c_config.mode =
        I2C_MODE_MASTER;


    i2c_config.sda_io_num =
        I2C_SDA_PIN;


    i2c_config.scl_io_num =
        I2C_SCL_PIN;


    i2c_config.sda_pullup_en =
        GPIO_PULLUP_ENABLE;


    i2c_config.scl_pullup_en =
        GPIO_PULLUP_ENABLE;


    i2c_config.master.clk_speed =
        I2C_FREQUENCY;


    /* =====================================================
     * APPLY I2C CONFIGURATION
     * ===================================================== */

    esp_err_t result =
        i2c_param_config(
            I2C_PORT,
            &i2c_config
        );


    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "I2C configuration failed"
        );


        return result;
    }


    /* =====================================================
     * INSTALL I2C DRIVER
     * ===================================================== */

    result =
        i2c_driver_install(
            I2C_PORT,
            I2C_MODE_MASTER,
            0,
            0,
            0
        );


    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "I2C driver installation failed"
        );


        return result;
    }


    /* =====================================================
     * START SCD41 PERIODIC MEASUREMENT
     * ===================================================== */

    result =
        send_command(
            SCD41_START_PERIODIC_MSB,
            SCD41_START_PERIODIC_LSB
        );


    if (result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Failed to start SCD41 measurement"
        );


        return result;
    }


    ESP_LOGI(
        TAG,
        "SCD41 periodic measurement started"
    );


    /*
     * The SCD41 requires several seconds before
     * the first periodic measurement becomes available.
     */
    vTaskDelay(
        pdMS_TO_TICKS(5000)
    );


    return ESP_OK;
}


/* =========================================================
 * READ REAL CO2 VALUE
 * ========================================================= */

co2_data_t co2_read(void)
{
    co2_data_t data = {};


    /*
     * Start with the previous valid value.
     *
     * Therefore every error path automatically keeps the
     * previous reading rather than returning zero.
     */
    data.ppm =
        last_valid_co2;


    /* =====================================================
     * REQUEST LATEST MEASUREMENT
     * ===================================================== */

    esp_err_t result =
        send_command(
            SCD41_READ_MEASUREMENT_MSB,
            SCD41_READ_MEASUREMENT_LSB
        );


    if (result != ESP_OK)
    {
        ESP_LOGW(
            TAG,
            "No new CO2 reading - keeping previous value %.0f ppm",
            last_valid_co2
        );


        return data;
    }


    /*
     * Short processing delay after sending the
     * read-measurement command.
     */
    vTaskDelay(
        pdMS_TO_TICKS(2)
    );


    /* =====================================================
     * READ SENSOR DATA
     * =====================================================
     *
     * SCD41 returns 9 bytes:
     *
     * Byte 0 : CO2 MSB
     * Byte 1 : CO2 LSB
     * Byte 2 : CO2 CRC
     *
     * Byte 3 : Temperature MSB
     * Byte 4 : Temperature LSB
     * Byte 5 : Temperature CRC
     *
     * Byte 6 : Humidity MSB
     * Byte 7 : Humidity LSB
     * Byte 8 : Humidity CRC
     */

    uint8_t buffer[9] = {0};


    result =
        i2c_master_read_from_device(
            I2C_PORT,
            SCD41_ADDRESS,
            buffer,
            sizeof(buffer),
            pdMS_TO_TICKS(1000)
        );


    if (result != ESP_OK)
    {
        ESP_LOGW(
            TAG,
            "Failed to read SCD41 - keeping previous value %.0f ppm",
            last_valid_co2
        );


        return data;
    }


    /* =====================================================
     * VERIFY CO2 CRC
     * ===================================================== */

    uint8_t expected_crc =
        calculate_crc(
            buffer,
            2
        );


    if (expected_crc != buffer[2])
    {
        ESP_LOGW(
            TAG,
            "CO2 CRC check failed - keeping previous value %.0f ppm",
            last_valid_co2
        );


        return data;
    }


    /* =====================================================
     * CONVERT RAW BYTES INTO CO2 PPM
     * =====================================================
     *
     * Example:
     *
     * MSB = 0x01
     * LSB = 0xF4
     *
     * 0x01F4 = 500
     *
     * Therefore:
     *
     * CO2 = 500 ppm
     */

    uint16_t co2_ppm =
        ((uint16_t)buffer[0] << 8) |
        buffer[1];


    /* =====================================================
     * SAVE NEW VALID VALUE
     * ===================================================== */

    last_valid_co2 =
        (float)co2_ppm;


    data.ppm =
        last_valid_co2;


    ESP_LOGI(
        TAG,
        "Real CO2: %.0f ppm",
        data.ppm
    );


    return data;
}