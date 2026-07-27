idf_component_register(
    SRCS
        "main.cpp"
        "system.cpp"
        "wifi.cpp"
        "sensor.cpp"
        "mqtt_wrapper.cpp"
        "json.cpp"

    INCLUDE_DIRS
        "."

    REQUIRES
        arduino
        mqtt
        scd4x
        i2cdev
)