#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"


/**
 * @brief Identifies the two control channels.
 */
typedef enum
{
    STORAGE_RELAY_1 = 0,
    STORAGE_RELAY_2

} storage_relay_t;


/* =========================================================
 * NVS INITIALIZATION
 * ========================================================= */

esp_err_t storage_init(void);


/* =========================================================
 * RELAY ON COUNTERS
 * ========================================================= */

uint32_t storage_get_count(
    storage_relay_t relay
);

void storage_increment_count(
    storage_relay_t relay
);


/* =========================================================
 * RELAY / LED LOGICAL STATES
 * ========================================================= */

esp_err_t storage_save_state(
    storage_relay_t relay,
    bool state
);

bool storage_get_state(
    storage_relay_t relay
);


/* =========================================================
 * WIFI CREDENTIALS
 * ========================================================= */

esp_err_t storage_save_wifi(
    const char *ssid,
    const char *password
);

bool storage_get_wifi(
    char *ssid,
    size_t ssid_size,
    char *password,
    size_t password_size
);


#endif