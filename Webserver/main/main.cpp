#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gpio/gpio.h"
#include "CO2/CO2.h"
#include "storage/storage.h"
#include "wifi/wifi.h"
#include "webserver/webserver.h"

#include "esp_log.h"


static const char *TAG = "MAIN";


/* =========================================================
 * GPIO MONITOR TASK
 * ========================================================= */

/**
 * @brief Continuously monitor GPIO5 and GPIO18.
 */
static void gpio_monitor_task(
    void *parameter)
{
    while (1)
    {
        gpio_control_update_inputs();


        vTaskDelay(
            pdMS_TO_TICKS(50)
        );
    }
}


/* =========================================================
 * APPLICATION MAIN
 * ========================================================= */

extern "C" void app_main(void)
{
    ESP_LOGI(
        TAG,
        "Starting ESP32 dashboard"
    );


    /* -----------------------------------------------------
     * 1. Initialize persistent NVS storage
     * ----------------------------------------------------- */

    ESP_ERROR_CHECK(
        storage_init()
    );


    /* -----------------------------------------------------
     * 2. Initialize relays, LEDs and GPIO controls
     *
     * Saved states are restored here.
     * ----------------------------------------------------- */

    ESP_ERROR_CHECK(
        gpio_control_init()
    );


    /* -----------------------------------------------------
     * 3. Initialize SCD41 CO2 sensor
     * ----------------------------------------------------- */

    esp_err_t co2_result =
        co2_init();


    if (co2_result != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "CO2 initialization failed"
        );
    }


    /* -----------------------------------------------------
     * 4. Initialize Wi-Fi
     *
     * Saved credentials are loaded from NVS.
     * ----------------------------------------------------- */

    ESP_ERROR_CHECK(
        wifi_init()
    );


    /*
     * Give Wi-Fi some time to establish the
     * initial connection before starting server.
     */
    vTaskDelay(
        pdMS_TO_TICKS(2000)
    );


    /* -----------------------------------------------------
     * 5. Start web dashboard
     * ----------------------------------------------------- */

    ESP_ERROR_CHECK(
        webserver_start()
    );


    /* -----------------------------------------------------
     * 6. Start physical GPIO monitoring
     * ----------------------------------------------------- */

    xTaskCreate(
        gpio_monitor_task,
        "gpio_monitor",
        2048,
        NULL,
        5,
        NULL
    );


    ESP_LOGI(
        TAG,
        "System initialization complete"
    );
}