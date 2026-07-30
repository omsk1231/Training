#ifndef CO2_H
#define CO2_H

#include "esp_err.h"


/* =========================================================
 * CO2 DATA STRUCTURE
 * ========================================================= */

/**
 * @brief Structure containing CO2 sensor data.
 */
typedef struct
{
    float ppm;

} co2_data_t;


/* =========================================================
 * PUBLIC FUNCTIONS
 * ========================================================= */

/**
 * @brief Initialize the SCD41 CO2 sensor.
 *
 * Initializes I2C and starts periodic measurement mode.
 *
 * @return
 *      ESP_OK on success,
 *      ESP_FAIL or another ESP-IDF error code on failure.
 */
esp_err_t co2_init(void);


/**
 * @brief Read the latest CO2 concentration.
 *
 * If a new valid measurement cannot be obtained,
 * the previously valid CO2 value is returned.
 *
 * @return CO2 data structure containing concentration in ppm.
 */
co2_data_t co2_read(void);


#endif