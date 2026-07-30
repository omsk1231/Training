#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "esp_err.h"


/**
 * @brief Start ESP32 HTTP dashboard server.
 */
esp_err_t webserver_start(void);


#endif