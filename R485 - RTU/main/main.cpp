#include "modbus.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


static const char *TAG = "MAIN";


extern "C" void app_main(void)
{
    // Storage for raw registers and decoded measurements
    modbus_data_t meter_data = {};


    // Initialize UART and Modbus RTU communication
    modbus_init();


    ESP_LOGI(TAG, "Starting voltmeter monitoring");


    while (1)
    {
        // Read Input Registers 0 to 15 from Modbus slave
        if (modbus_read_registers(&meter_data))
        {
            // Convert every two 16-bit registers
            // into one 32-bit floating-point value
            modbus_decode_values(&meter_data);


            // Display raw registers and decoded measurements
            modbus_print_data(&meter_data);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to read voltmeter");
        }


        // Read the voltmeter once every second
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}