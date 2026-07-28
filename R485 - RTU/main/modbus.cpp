#include "modbus.h"

#include <stdio.h>
#include <string.h>

#include "driver/uart.h"
#include "esp_log.h"


// =====================================================
// LOCAL CONFIGURATION
// =====================================================

static const char *TAG = "MODBUS";

#define MODBUS_UART UART_NUM_2

// Response:
//
// Slave ID        1 byte
// Function Code   1 byte
// Byte Count      1 byte
// Register Data  32 bytes
// CRC              2 bytes
//
// Total = 37 bytes

#define MODBUS_RESPONSE_SIZE 37


// =====================================================
// CRC16 CALCULATION
// =====================================================

static uint16_t calculate_crc16(const uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < length; i++)
    {
        crc ^= data[i];

        for (uint8_t bit = 0; bit < 8; bit++)
        {
            if (crc & 0x0001)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }

    return crc;
}


// =====================================================
// CONVERT TWO REGISTERS INTO FLOAT
// =====================================================
//
// Our voltmeter stores a 32-bit float in two
// consecutive 16-bit registers.
//
// Example:
//
// Register 0 = Low word
// Register 1 = High word
//
// Final 32-bit data:
//
// [ Register 1 ][ Register 0 ]
//
// =====================================================

static float register_pair_to_float(uint16_t low_word,
                                    uint16_t high_word)
{
    uint32_t raw_value;

    raw_value =
        ((uint32_t)high_word << 16) |
        (uint32_t)low_word;

    float value;

    memcpy(&value, &raw_value, sizeof(value));

    return value;
}


// =====================================================
// INITIALIZE MODBUS UART
// =====================================================

void modbus_init(void)
{
    uart_config_t uart_config = {};

    uart_config.baud_rate = MODBUS_BAUD_RATE;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.source_clk = UART_SCLK_DEFAULT;


    // Install UART driver

    ESP_ERROR_CHECK(
        uart_driver_install(
            MODBUS_UART,
            256,
            256,
            0,
            NULL,
            0
        )
    );


    // Apply UART configuration

    ESP_ERROR_CHECK(
        uart_param_config(
            MODBUS_UART,
            &uart_config
        )
    );


    // Configure UART pins connected internally
    // to the onboard RS485 transceiver

    ESP_ERROR_CHECK(
        uart_set_pin(
            MODBUS_UART,
            MODBUS_TX_GPIO,
            MODBUS_RX_GPIO,
            UART_PIN_NO_CHANGE,
            UART_PIN_NO_CHANGE
        )
    );


    ESP_LOGI(
        TAG,
        "Modbus initialized - Slave:%d Baud:%d TX:%d RX:%d",
        MODBUS_SLAVE_ID,
        MODBUS_BAUD_RATE,
        MODBUS_TX_GPIO,
        MODBUS_RX_GPIO
    );
}


// =====================================================
// READ INPUT REGISTERS
// =====================================================

bool modbus_read_registers(modbus_data_t *data)
{
    if (data == NULL)
    {
        return false;
    }


    // -------------------------------------------------
    // Create Modbus request
    // -------------------------------------------------

    uint8_t request[8];

    request[0] = MODBUS_SLAVE_ID;
    request[1] = MODBUS_FUNCTION_CODE;

    // Starting register address

    request[2] =
        (MODBUS_START_ADDRESS >> 8) & 0xFF;

    request[3] =
        MODBUS_START_ADDRESS & 0xFF;


    // Number of registers

    request[4] =
        (MODBUS_REG_COUNT >> 8) & 0xFF;

    request[5] =
        MODBUS_REG_COUNT & 0xFF;


    // -------------------------------------------------
    // Calculate request CRC
    // -------------------------------------------------

    uint16_t crc =
        calculate_crc16(request, 6);

    // Modbus RTU sends CRC low byte first

    request[6] = crc & 0xFF;
    request[7] = (crc >> 8) & 0xFF;


    // -------------------------------------------------
    // Clear previous UART data
    // -------------------------------------------------

    uart_flush_input(MODBUS_UART);


    // -------------------------------------------------
    // Send request to voltmeter
    // -------------------------------------------------

    int bytes_sent =
        uart_write_bytes(
            MODBUS_UART,
            (const char *)request,
            sizeof(request)
        );

    if (bytes_sent != sizeof(request))
    {
        ESP_LOGE(TAG, "Failed to send Modbus request");
        return false;
    }


    uart_wait_tx_done(
        MODBUS_UART,
        pdMS_TO_TICKS(100)
    );


    // -------------------------------------------------
    // Receive response
    // -------------------------------------------------

    uint8_t response[MODBUS_RESPONSE_SIZE];

    int bytes_received =
        uart_read_bytes(
            MODBUS_UART,
            response,
            sizeof(response),
            pdMS_TO_TICKS(1000)
        );


    if (bytes_received != MODBUS_RESPONSE_SIZE)
    {
        ESP_LOGE(
            TAG,
            "Expected %d bytes, received %d",
            MODBUS_RESPONSE_SIZE,
            bytes_received
        );

        return false;
    }


    // -------------------------------------------------
    // Check Slave ID
    // -------------------------------------------------

    if (response[0] != MODBUS_SLAVE_ID)
    {
        ESP_LOGE(TAG, "Incorrect Slave ID");
        return false;
    }


    // -------------------------------------------------
    // Check Function Code
    // -------------------------------------------------

    if (response[1] != MODBUS_FUNCTION_CODE)
    {
        ESP_LOGE(
            TAG,
            "Unexpected Function Code: 0x%02X",
            response[1]
        );

        return false;
    }


    // -------------------------------------------------
    // Check returned byte count
    // -------------------------------------------------

    if (response[2] != (MODBUS_REG_COUNT * 2))
    {
        ESP_LOGE(TAG, "Incorrect register byte count");
        return false;
    }


    // -------------------------------------------------
    // Check response CRC
    // -------------------------------------------------

    uint16_t received_crc =
        response[MODBUS_RESPONSE_SIZE - 2] |
        ((uint16_t)response[MODBUS_RESPONSE_SIZE - 1] << 8);


    uint16_t calculated_crc =
        calculate_crc16(
            response,
            MODBUS_RESPONSE_SIZE - 2
        );


    if (received_crc != calculated_crc)
    {
        ESP_LOGE(TAG, "CRC verification failed");
        return false;
    }


    // -------------------------------------------------
    // Extract 16 registers from response
    // -------------------------------------------------

    for (int i = 0; i < MODBUS_REG_COUNT; i++)
    {
        data->registers[i] =
            ((uint16_t)response[3 + (i * 2)] << 8) |
            response[4 + (i * 2)];
    }


    return true;
}


// =====================================================
// DECODE REGISTER VALUES
// =====================================================

void modbus_decode_values(modbus_data_t *data)
{
    if (data == NULL)
    {
        return;
    }


    // Every measurement occupies two registers:
    //
    // 0  + 1  -> Value 1
    // 2  + 3  -> Value 2
    // 4  + 5  -> Value 3
    // ...
    // 14 + 15 -> Value 8

    for (int i = 0; i < MODBUS_VALUE_COUNT; i++)
    {
        int register_index = i * 2;

        data->values[i] =
            register_pair_to_float(
                data->registers[register_index],
                data->registers[register_index + 1]
            );
    }
}


// =====================================================
// DISPLAY DATA
// =====================================================

void modbus_print_data(const modbus_data_t *data)
{
    if (data == NULL)
    {
        return;
    }


    printf("\n");
    printf("========== RAW REGISTERS ==========\n");


    for (int i = 0; i < MODBUS_REG_COUNT; i++)
    {
        printf(
            "Register %-2d : %u\n",
            i,
            data->registers[i]
        );
    }


    printf("\n");
    printf("========= DECODED VALUES ==========\n");


    for (int i = 0; i < MODBUS_VALUE_COUNT; i++)
    {
        printf(
            "Value %-2d : %.2f\n",
            i + 1,
            data->values[i]
        );
    }


    printf("===================================\n");
}