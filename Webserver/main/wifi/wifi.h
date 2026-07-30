#ifndef WIFI_H
#define WIFI_H

#include "esp_err.h"


/**
 * @brief Initialize Wi-Fi.
 *
 * Loads credentials from NVS when available.
 */
esp_err_t wifi_init(void);


/**
 * @brief Save new credentials and connect to new network.
 */
esp_err_t wifi_change_credentials(
    const char *ssid,
    const char *password
);


#endif