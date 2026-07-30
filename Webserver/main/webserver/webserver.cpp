#include "webserver/webserver.h"

#include <stdio.h>
#include <string.h>

#include "CO2/CO2.h"
#include "gpio/gpio.h"
#include "wifi/wifi.h"

#include "esp_http_server.h"
#include "esp_log.h"


static const char *TAG = "WEBSERVER";


/* =========================================================
 * DASHBOARD HTML
 * ========================================================= */

static const char DASHBOARD_HTML[] =
R"rawliteral(
<!DOCTYPE html>

<html>

<head>

<meta charset="UTF-8">

<meta
    name="viewport"
    content="width=device-width, initial-scale=1.0">

<title>ESP32 Dashboard</title>

<style>

body
{
    font-family: Arial, sans-serif;
    background: #f1f3f5;
    text-align: center;
    margin: 0;
    padding: 30px;
}

h1
{
    color: #333;
}

.card
{
    background: white;
    max-width: 550px;
    margin: 15px auto;
    padding: 25px;
    border-radius: 10px;

    box-shadow:
        0 2px 8px
        rgba(0,0,0,0.15);
}

.co2
{
    font-size: 38px;
    font-weight: bold;
}

.status
{
    font-size: 18px;
    font-weight: bold;
}

button
{
    border: none;
    color: white;
    padding: 12px 22px;
    margin: 8px;
    border-radius: 5px;
    cursor: pointer;
    font-size: 15px;
}

.on
{
    background: #28a745;
}

.off
{
    background: #dc3545;
}

.wifi
{
    background: #007bff;
}

input
{
    width: 80%;
    max-width: 350px;
    padding: 11px;
    margin: 6px;
    border: 1px solid #ccc;
    border-radius: 5px;
}

#wifi_message
{
    font-weight: bold;
}

</style>

</head>


<body>


<h1>ESP32 Control Dashboard</h1>


<div class="card">

    <h2>CO2 Monitoring</h2>

    <div class="co2">

        <span id="co2">---</span> ppm

    </div>

</div>


<div class="card">

    <h2>GPIO 1 / Relay 1</h2>

    <p>
        Status:
        <span
            id="status1"
            class="status">
            OFF
        </span>
    </p>


    <button
        class="on"
        onclick="writeControl(1,1)">
        WRITE ON
    </button>


    <button
        class="off"
        onclick="writeControl(1,0)">
        WRITE OFF
    </button>


    <p>
        Relay 1 ON Count:
        <span id="count1">0</span>
    </p>

</div>


<div class="card">

    <h2>GPIO 2 / Relay 2</h2>

    <p>
        Status:
        <span
            id="status2"
            class="status">
            OFF
        </span>
    </p>


    <button
        class="on"
        onclick="writeControl(2,1)">
        WRITE ON
    </button>


    <button
        class="off"
        onclick="writeControl(2,0)">
        WRITE OFF
    </button>


    <p>
        Relay 2 ON Count:
        <span id="count2">0</span>
    </p>

</div>


<div class="card">

    <h2>Wi-Fi Settings</h2>

    <p>
        Change the Wi-Fi network used by ESP32.
    </p>


    <input
        id="wifi_ssid"
        type="text"
        maxlength="32"
        placeholder="Wi-Fi SSID">


    <br>


    <input
        id="wifi_password"
        type="password"
        maxlength="64"
        placeholder="Wi-Fi Password">


    <br>


    <button
        class="wifi"
        onclick="changeWifi()">
        SAVE & CONNECT
    </button>


    <p id="wifi_message"></p>

</div>


<script>

/* =========================================================
 * READ DASHBOARD STATUS EVERY SECOND
 * ========================================================= */

async function updateStatus()
{
    try
    {
        const response =
            await fetch(
                "/api/status",
                {
                    cache: "no-store"
                }
            );


        if (!response.ok)
        {
            return;
        }


        const data =
            await response.json();


        document.getElementById(
            "co2"
        ).innerText =
            Number(data.co2).toFixed(0);


        document.getElementById(
            "status1"
        ).innerText =
            data.control1
                ? "ON"
                : "OFF";


        document.getElementById(
            "status2"
        ).innerText =
            data.control2
                ? "ON"
                : "OFF";


        document.getElementById(
            "count1"
        ).innerText =
            data.count1;


        document.getElementById(
            "count2"
        ).innerText =
            data.count2;
    }
    catch (error)
    {
        console.log(
            "Status update failed"
        );
    }
}


/* =========================================================
 * WRITE GPIO CONTROL
 * ========================================================= */

async function writeControl(
    control,
    state)
{
    try
    {
        await fetch(
            "/api/control"
            + "?channel="
            + control
            + "&state="
            + state,
            {
                method: "POST"
            }
        );


        updateStatus();
    }
    catch (error)
    {
        console.log(
            "Control request failed"
        );
    }
}


/* =========================================================
 * CHANGE WIFI
 * ========================================================= */

async function changeWifi()
{
    const ssid =
        document.getElementById(
            "wifi_ssid"
        ).value;


    const password =
        document.getElementById(
            "wifi_password"
        ).value;


    const message =
        document.getElementById(
            "wifi_message"
        );


    if (ssid.length === 0)
    {
        message.innerText =
            "Please enter Wi-Fi SSID";

        return;
    }


    message.innerText =
        "Saving credentials...";


    const body =
        "ssid="
        + encodeURIComponent(ssid)
        + "&password="
        + encodeURIComponent(password);


    try
    {
        const response =
            await fetch(
                "/api/wifi",
                {
                    method: "POST",

                    headers:
                    {
                        "Content-Type":
                        "application/x-www-form-urlencoded"
                    },

                    body: body
                }
            );


        if (response.ok)
        {
            message.innerText =
                "Saved. ESP32 is switching Wi-Fi...";
        }
        else
        {
            message.innerText =
                "Failed to change Wi-Fi";
        }
    }
    catch (error)
    {
        /*
         * A disconnect here can actually be expected,
         * because ESP32 may already be switching Wi-Fi.
         */
        message.innerText =
            "ESP32 is switching networks...";
    }
}


updateStatus();


setInterval(
    updateStatus,
    1000
);

</script>


</body>

</html>
)rawliteral";


/* =========================================================
 * ROOT PAGE
 * ========================================================= */

static esp_err_t root_handler(
    httpd_req_t *req)
{
    httpd_resp_set_type(
        req,
        "text/html"
    );


    return httpd_resp_send(
        req,
        DASHBOARD_HTML,
        HTTPD_RESP_USE_STRLEN
    );
}


/* =========================================================
 * STATUS API
 * ========================================================= */

static esp_err_t status_handler(
    httpd_req_t *req)
{
    gpio_status_t status =
        gpio_control_get_status();


    co2_data_t co2 =
        co2_read();


    char json[256];


    snprintf(
        json,
        sizeof(json),

        "{"
        "\"co2\":%.0f,"
        "\"control1\":%d,"
        "\"control2\":%d,"
        "\"count1\":%lu,"
        "\"count2\":%lu"
        "}",

        co2.ppm,

        status.gpio1_state == OUTPUT_ON,

        status.gpio2_state == OUTPUT_ON,

        (unsigned long)
            status.relay1_count,

        (unsigned long)
            status.relay2_count
    );


    httpd_resp_set_type(
        req,
        "application/json"
    );


    return httpd_resp_sendstr(
        req,
        json
    );
}


/* =========================================================
 * CONTROL WRITE API
 * ========================================================= */

static esp_err_t control_handler(
    httpd_req_t *req)
{
    char query[100] = {0};

    char channel_string[10] = {0};

    char state_string[10] = {0};


    size_t query_length =
        httpd_req_get_url_query_len(
            req
        );


    if (query_length == 0 ||
        query_length >= sizeof(query))
    {
        httpd_resp_send_err(
            req,
            HTTPD_400_BAD_REQUEST,
            "Missing parameters"
        );

        return ESP_FAIL;
    }


    httpd_req_get_url_query_str(
        req,
        query,
        sizeof(query)
    );


    if (httpd_query_key_value(
            query,
            "channel",
            channel_string,
            sizeof(channel_string))
            != ESP_OK ||
        httpd_query_key_value(
            query,
            "state",
            state_string,
            sizeof(state_string))
            != ESP_OK)
    {
        httpd_resp_send_err(
            req,
            HTTPD_400_BAD_REQUEST,
            "Invalid parameters"
        );

        return ESP_FAIL;
    }


    int channel =
        atoi(channel_string);

    int state =
        atoi(state_string);


    output_state_t requested_state =
        state
            ? OUTPUT_ON
            : OUTPUT_OFF;


    if (channel == 1)
    {
        gpio_control_set_1(
            requested_state
        );
    }
    else if (channel == 2)
    {
        gpio_control_set_2(
            requested_state
        );
    }
    else
    {
        httpd_resp_send_err(
            req,
            HTTPD_400_BAD_REQUEST,
            "Invalid channel"
        );

        return ESP_FAIL;
    }


    return httpd_resp_sendstr(
        req,
        "OK"
    );
}


/* =========================================================
 * WIFI API
 * ========================================================= */

static esp_err_t wifi_handler(
    httpd_req_t *req)
{
    char content[200] = {0};


    if (req->content_len <= 0 ||
        req->content_len >= sizeof(content))
    {
        httpd_resp_send_err(
            req,
            HTTPD_400_BAD_REQUEST,
            "Invalid request"
        );

        return ESP_FAIL;
    }


    int received =
        httpd_req_recv(
            req,
            content,
            req->content_len
        );


    if (received <= 0)
    {
        httpd_resp_send_err(
            req,
            HTTPD_400_BAD_REQUEST,
            "Receive failed"
        );

        return ESP_FAIL;
    }


    content[received] = '\0';


    char ssid[33] = {0};

    char password[65] = {0};


    if (httpd_query_key_value(
            content,
            "ssid",
            ssid,
            sizeof(ssid))
            != ESP_OK ||
        httpd_query_key_value(
            content,
            "password",
            password,
            sizeof(password))
            != ESP_OK)
    {
        httpd_resp_send_err(
            req,
            HTTPD_400_BAD_REQUEST,
            "Missing Wi-Fi credentials"
        );

        return ESP_FAIL;
    }


    /*
     * Send response before changing network.
     */
    httpd_resp_set_type(
        req,
        "text/plain"
    );


    httpd_resp_sendstr(
        req,
        "Wi-Fi credentials saved"
    );


    ESP_LOGI(
        TAG,
        "Wi-Fi change requested"
    );


    wifi_change_credentials(
        ssid,
        password
    );


    return ESP_OK;
}


/* =========================================================
 * START SERVER
 * ========================================================= */

esp_err_t webserver_start(void)
{
    httpd_handle_t server = NULL;


    httpd_config_t config =
        HTTPD_DEFAULT_CONFIG();


    ESP_LOGI(
        TAG,
        "Starting HTTP server"
    );


    esp_err_t err =
        httpd_start(
            &server,
            &config
        );


    if (err != ESP_OK)
    {
        return err;
    }


    /* Root page */

    httpd_uri_t root_uri =
    {
        .uri = "/",
        .method = HTTP_GET,
        .handler = root_handler,
        .user_ctx = NULL
    };


    /* Status */

    httpd_uri_t status_uri =
    {
        .uri = "/api/status",
        .method = HTTP_GET,
        .handler = status_handler,
        .user_ctx = NULL
    };


    /* GPIO write */

    httpd_uri_t control_uri =
    {
        .uri = "/api/control",
        .method = HTTP_POST,
        .handler = control_handler,
        .user_ctx = NULL
    };


    /* Wi-Fi configuration */

    httpd_uri_t wifi_uri =
    {
        .uri = "/api/wifi",
        .method = HTTP_POST,
        .handler = wifi_handler,
        .user_ctx = NULL
    };


    httpd_register_uri_handler(
        server,
        &root_uri
    );


    httpd_register_uri_handler(
        server,
        &status_uri
    );


    httpd_register_uri_handler(
        server,
        &control_uri
    );


    httpd_register_uri_handler(
        server,
        &wifi_uri
    );


    ESP_LOGI(
        TAG,
        "Dashboard server started"
    );


    return ESP_OK;
}