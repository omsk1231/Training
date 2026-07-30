#ifndef GPIO_CONTROL_H
#define GPIO_CONTROL_H

#include <stdint.h>

#include "esp_err.h"


/* =========================================================
 * LOGICAL OUTPUT STATE
 * ========================================================= */

typedef enum
{
    OUTPUT_OFF = 0,
    OUTPUT_ON  = 1

} output_state_t;


/* =========================================================
 * GPIO STATUS STRUCTURE
 * ========================================================= */

typedef struct
{
    output_state_t gpio1_state;
    output_state_t gpio2_state;

    uint32_t relay1_count;
    uint32_t relay2_count;

} gpio_status_t;


/* =========================================================
 * PUBLIC FUNCTIONS
 * ========================================================= */

/**
 * @brief Initialize GPIO inputs, relays and LEDs.
 *
 * Saved logical states are restored from NVS.
 */
esp_err_t gpio_control_init(void);


/**
 * @brief Monitor physical GPIO5 and GPIO18.
 */
void gpio_control_update_inputs(void);


/**
 * @brief Dashboard write for Control 1.
 */
void gpio_control_set_1(
    output_state_t state
);


/**
 * @brief Dashboard write for Control 2.
 */
void gpio_control_set_2(
    output_state_t state
);


/**
 * @brief Obtain current GPIO status.
 */
gpio_status_t gpio_control_get_status(void);


#endif