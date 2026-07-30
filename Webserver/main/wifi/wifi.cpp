#include "wifi/wifi.h"
#include "storage/storage.h"

#include <string.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"


static const char *TAG = "WIFI";


/* =========================================================
 * DEFAULT WIFI
 * =========================================================
 *
 * Used only when NVS does not contain credentials yet.
 */

#define DEFAULT_WIFI_SSID       "IBMS1"
#define DEFAULT_WIFI_PASSWORD   "123456789"


#define MAX_WIFI_RETRY 10


static int retry_count = 0;


/* =========================================================
 * WIFI EVENT HANDLER
 * ========================================================= */

static void wifi_event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data)
{
    if (event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(
            TAG,
            "Wi-Fi started"
        );

        esp_wifi_connect();
    }


    else if (
        event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (retry_count < MAX_WIFI_RETRY)
        {
            retry_count++;

            ESP_LOGW(
                TAG,
                "Disconnected. Retry %d/%d",
                retry_count,
                MAX_WIFI_RETRY
            );

            esp_wifi_connect();
        }
        else
        {
            ESP_LOGE(
                TAG,
                "Unable to connect to Wi-Fi"
            );
        }
    }


    else if (
        event_base == IP_EVENT &&
        event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event =
            (ip_event_got_ip_t *)event_data;


        retry_count = 0;


        ESP_LOGI(
            TAG,
            "Connected"
        );


        ESP_LOGI(
            TAG,
            "ESP32 IP: " IPSTR,
            IP2STR(
                &event->ip_info.ip
            )
        );
    }
}


/* =========================================================
 * INITIALIZE WIFI
 * ========================================================= */

esp_err_t wifi_init(void)
{
    char ssid[33] = {0};

    char password[65] = {0};


    /* -----------------------------------------------------
     * Try reading credentials from NVS
     * ----------------------------------------------------- */

    bool credentials_exist =
        storage_get_wifi(
            ssid,
            sizeof(ssid),
            password,
            sizeof(password)
        );


    if (!credentials_exist)
    {
        /*
         * First boot.
         *
         * Copy compiled default credentials.
         */

        strncpy(
            ssid,
            DEFAULT_WIFI_SSID,
            sizeof(ssid) - 1
        );


        strncpy(
            password,
            DEFAULT_WIFI_PASSWORD,
            sizeof(password) - 1
        );


        /*
         * Save defaults so future boots use NVS.
         */

        storage_save_wifi(
            ssid,
            password
        );


        ESP_LOGI(
            TAG,
            "No saved Wi-Fi found"
        );

        ESP_LOGI(
            TAG,
            "Using default Wi-Fi: %s",
            ssid
        );
    }
    else
    {
        ESP_LOGI(
            TAG,
            "Using Wi-Fi credentials stored in NVS"
        );

        ESP_LOGI(
            TAG,
            "SSID: %s",
            ssid
        );
    }


    /* -----------------------------------------------------
     * Initialize network stack
     * ----------------------------------------------------- */

    esp_err_t err =
        esp_netif_init();


    if (err != ESP_OK &&
        err != ESP_ERR_INVALID_STATE)
    {
        return err;
    }


    err =
        esp_event_loop_create_default();


    if (err != ESP_OK &&
        err != ESP_ERR_INVALID_STATE)
    {
        return err;
    }


    esp_netif_create_default_wifi_sta();


    wifi_init_config_t wifi_init_config =
        WIFI_INIT_CONFIG_DEFAULT();


    ESP_ERROR_CHECK(
        esp_wifi_init(
            &wifi_init_config
        )
    );


    /* -----------------------------------------------------
     * Register events
     * ----------------------------------------------------- */

    ESP_ERROR_CHECK(
        esp_event_handler_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &wifi_event_handler,
            NULL
        )
    );


    ESP_ERROR_CHECK(
        esp_event_handler_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &wifi_event_handler,
            NULL
        )
    );


    /* -----------------------------------------------------
     * Configure Wi-Fi station
     * ----------------------------------------------------- */

    wifi_config_t wifi_config = {};


    strncpy(
        (char *)wifi_config.sta.ssid,
        ssid,
        sizeof(wifi_config.sta.ssid) - 1
    );


    strncpy(
        (char *)wifi_config.sta.password,
        password,
        sizeof(wifi_config.sta.password) - 1
    );


    ESP_ERROR_CHECK(
        esp_wifi_set_mode(
            WIFI_MODE_STA
        )
    );


    ESP_ERROR_CHECK(
        esp_wifi_set_config(
            WIFI_IF_STA,
            &wifi_config
        )
    );


    ESP_ERROR_CHECK(
        esp_wifi_start()
    );


    ESP_LOGI(
        TAG,
        "Connecting to %s...",
        ssid
    );


    return ESP_OK;
}


/* =========================================================
 * CHANGE WIFI CREDENTIALS
 * ========================================================= */

esp_err_t wifi_change_credentials(
    const char *ssid,
    const char *password)
{
    if (ssid == NULL ||
        password == NULL ||
        strlen(ssid) == 0)
    {
        return ESP_ERR_INVALID_ARG;
    }


    /*
     * Validate lengths.
     */
    if (strlen(ssid) > 32 ||
        strlen(password) > 64)
    {
        return ESP_ERR_INVALID_ARG;
    }


    ESP_LOGI(
        TAG,
        "New Wi-Fi requested: %s",
        ssid
    );


    /*
     * Save credentials permanently first.
     */
    esp_err_t err =
        storage_save_wifi(
            ssid,
            password
        );


    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Failed to save Wi-Fi credentials"
        );

        return err;
    }


    wifi_config_t wifi_config = {};


    strncpy(
        (char *)wifi_config.sta.ssid,
        ssid,
        sizeof(wifi_config.sta.ssid) - 1
    );


    strncpy(
        (char *)wifi_config.sta.password,
        password,
        sizeof(wifi_config.sta.password) - 1
    );


    /*
     * Disconnect current Wi-Fi.
     */
    esp_wifi_disconnect();


    /*
     * Apply new configuration.
     */
    err =
        esp_wifi_set_config(
            WIFI_IF_STA,
            &wifi_config
        );


    if (err != ESP_OK)
    {
        return err;
    }


    retry_count = 0;


    /*
     * Connect to new network.
     */
    err =
        esp_wifi_connect();


    return err;
}