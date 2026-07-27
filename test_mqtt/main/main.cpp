#include <Arduino.h>

#include "system.h"

void setup()
{
    Serial.begin(115200);

    delay(1000);

    if (!system_init())
    {
        Serial.println("System Initialization Failed");

        while (true)
        {
            delay(1000);
        }
    }
}

void loop()
{
    system_run();

    delay(5000);
}

extern "C" void app_main(void)
{
    initArduino();

    setup();

    while (true)
    {
        loop();
    }
}