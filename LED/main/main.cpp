#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_1 GPIO_NUM_2
#define LED_2 GPIO_NUM_15

extern "C" void app_main(void)
{
    // Configure LED pins as outputs
    gpio_reset_pin(LED_1);
    gpio_reset_pin(LED_2);

    gpio_set_direction(LED_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_2, GPIO_MODE_OUTPUT);

    while (1)
    {
        // LED1 ON, LED2 OFF
        gpio_set_level(LED_1, 1);
        gpio_set_level(LED_2, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));

        // LED1 OFF, LED2 ON
        gpio_set_level(LED_1, 0);
        gpio_set_level(LED_2, 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}