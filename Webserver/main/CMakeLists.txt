idf_component_register(
    SRCS
        "main.cpp"
        "CO2/CO2.cpp"
        "gpio/gpio.cpp"
        "storage/storage.cpp"
        "wifi/wifi.cpp"
        "webserver/webserver.cpp"

    INCLUDE_DIRS
        "."
        "CO2"
        "gpio"
        "storage"
        "wifi"
        "webserver"

    REQUIRES
        driver
        esp_wifi
        esp_event
        esp_netif
        nvs_flash
        esp_http_server
)