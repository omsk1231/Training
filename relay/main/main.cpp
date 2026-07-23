#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define INPUT1  GPIO_NUM_5
#define INPUT2  GPIO_NUM_18

#define RELAY1  GPIO_NUM_14
#define RELAY2  GPIO_NUM_13

extern "C" void app_main(void)
{
    // Configure inputs
    gpio_reset_pin(INPUT1);
    gpio_set_direction(INPUT1, GPIO_MODE_INPUT);

    gpio_reset_pin(INPUT2);
    gpio_set_direction(INPUT2, GPIO_MODE_INPUT);

    // Configure relay outputs
    gpio_reset_pin(RELAY1);
    gpio_set_direction(RELAY1, GPIO_MODE_OUTPUT);

    gpio_reset_pin(RELAY2);
    gpio_set_direction(RELAY2, GPIO_MODE_OUTPUT);

    while (1)
    {
        int input1 = gpio_get_level(INPUT1);
        int input2 = gpio_get_level(INPUT2);

        // Relay 1
        if (input1 == 0)
            gpio_set_level(RELAY1, 1);   // ON
        else
            gpio_set_level(RELAY1, 0);   // OFF

        // Relay 2
        if (input2 == 0)
            gpio_set_level(RELAY2, 1);   // ON
        else
            gpio_set_level(RELAY2, 0);   // OFF

        printf("IN1=%d  RELAY1=%d | IN2=%d  RELAY2=%d\n",
               input1,
               (input1 == 0),
               input2,
               (input2 == 0));

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}