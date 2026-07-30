#include "storage/storage.h"

#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"


static const char *TAG = "STORAGE";

#define NVS_NAMESPACE "dashboard"


/* =========================================================
 * INITIALIZE NVS
 * ========================================================= */

esp_err_t storage_init(void)
{
    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_LOGW(
            TAG,
            "NVS partition needs erase"
        );

        ESP_ERROR_CHECK(
            nvs_flash_erase()
        );

        err = nvs_flash_init();
    }


    if (err == ESP_OK)
    {
        ESP_LOGI(
            TAG,
            "NVS initialized successfully"
        );
    }
    else
    {
        ESP_LOGE(
            TAG,
            "NVS initialization failed: %s",
            esp_err_to_name(err)
        );
    }


    return err;
}


/* =========================================================
 * GET RELAY ON COUNTER
 * ========================================================= */

uint32_t storage_get_count(
    storage_relay_t relay)
{
    nvs_handle_t handle;

    uint32_t count = 0;


    const char *key =
        (relay == STORAGE_RELAY_1)
            ? "r1_count"
            : "r2_count";


    esp_err_t err =
        nvs_open(
            NVS_NAMESPACE,
            NVS_READONLY,
            &handle
        );


    if (err != ESP_OK)
    {
        return 0;
    }


    err =
        nvs_get_u32(
            handle,
            key,
            &count
        );


    /*
     * Key not existing simply means that the
     * relay has never been switched ON before.
     */
    if (err == ESP_ERR_NVS_NOT_FOUND)
    {
        count = 0;
    }


    nvs_close(handle);

    return count;
}


/* =========================================================
 * INCREMENT RELAY COUNTER
 * ========================================================= */

void storage_increment_count(
    storage_relay_t relay)
{
    nvs_handle_t handle;


    uint32_t count =
        storage_get_count(relay);


    count++;


    const char *key =
        (relay == STORAGE_RELAY_1)
            ? "r1_count"
            : "r2_count";


    esp_err_t err =
        nvs_open(
            NVS_NAMESPACE,
            NVS_READWRITE,
            &handle
        );


    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Unable to open NVS counter"
        );

        return;
    }


    err =
        nvs_set_u32(
            handle,
            key,
            count
        );


    if (err == ESP_OK)
    {
        err = nvs_commit(handle);
    }


    nvs_close(handle);


    if (err == ESP_OK)
    {
        ESP_LOGI(
            TAG,
            "%s = %lu",
            key,
            (unsigned long)count
        );
    }
}


/* =========================================================
 * SAVE LOGICAL RELAY / LED STATE
 * ========================================================= */

esp_err_t storage_save_state(
    storage_relay_t relay,
    bool state)
{
    nvs_handle_t handle;


    const char *key =
        (relay == STORAGE_RELAY_1)
            ? "r1_state"
            : "r2_state";


    esp_err_t err =
        nvs_open(
            NVS_NAMESPACE,
            NVS_READWRITE,
            &handle
        );


    if (err != ESP_OK)
    {
        return err;
    }


    err =
        nvs_set_u8(
            handle,
            key,
            state ? 1 : 0
        );


    if (err == ESP_OK)
    {
        err = nvs_commit(handle);
    }


    nvs_close(handle);


    if (err == ESP_OK)
    {
        ESP_LOGI(
            TAG,
            "%s saved as %s",
            key,
            state ? "ON" : "OFF"
        );
    }


    return err;
}


/* =========================================================
 * READ SAVED LOGICAL STATE
 * ========================================================= */

bool storage_get_state(
    storage_relay_t relay)
{
    nvs_handle_t handle;

    uint8_t value = 0;


    const char *key =
        (relay == STORAGE_RELAY_1)
            ? "r1_state"
            : "r2_state";


    esp_err_t err =
        nvs_open(
            NVS_NAMESPACE,
            NVS_READONLY,
            &handle
        );


    if (err != ESP_OK)
    {
        return false;
    }


    err =
        nvs_get_u8(
            handle,
            key,
            &value
        );


    nvs_close(handle);


    /*
     * First boot:
     * no saved state exists.
     *
     * Default = OFF.
     */
    if (err != ESP_OK)
    {
        return false;
    }


    return (value == 1);
}


/* =========================================================
 * SAVE WIFI CREDENTIALS
 * ========================================================= */

esp_err_t storage_save_wifi(
    const char *ssid,
    const char *password)
{
    if (ssid == NULL ||
        password == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }


    nvs_handle_t handle;


    esp_err_t err =
        nvs_open(
            NVS_NAMESPACE,
            NVS_READWRITE,
            &handle
        );


    if (err != ESP_OK)
    {
        return err;
    }


    err =
        nvs_set_str(
            handle,
            "wifi_ssid",
            ssid
        );


    if (err == ESP_OK)
    {
        err =
            nvs_set_str(
                handle,
                "wifi_pass",
                password
            );
    }


    if (err == ESP_OK)
    {
        err =
            nvs_commit(handle);
    }


    nvs_close(handle);


    if (err == ESP_OK)
    {
        ESP_LOGI(
            TAG,
            "Wi-Fi credentials saved in NVS"
        );
    }


    return err;
}


/* =========================================================
 * READ WIFI CREDENTIALS
 * ========================================================= */

bool storage_get_wifi(
    char *ssid,
    size_t ssid_size,
    char *password,
    size_t password_size)
{
    if (ssid == NULL ||
        password == NULL)
    {
        return false;
    }


    nvs_handle_t handle;


    esp_err_t err =
        nvs_open(
            NVS_NAMESPACE,
            NVS_READONLY,
            &handle
        );


    if (err != ESP_OK)
    {
        return false;
    }


    size_t stored_ssid_size =
        ssid_size;

    size_t stored_password_size =
        password_size;


    esp_err_t ssid_err =
        nvs_get_str(
            handle,
            "wifi_ssid",
            ssid,
            &stored_ssid_size
        );


    esp_err_t password_err =
        nvs_get_str(
            handle,
            "wifi_pass",
            password,
            &stored_password_size
        );


    nvs_close(handle);


    if (ssid_err != ESP_OK ||
        password_err != ESP_OK)
    {
        return false;
    }


    return true;
}