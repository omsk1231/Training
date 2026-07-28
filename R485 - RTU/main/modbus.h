#ifndef MODBUS_H
#define MODBUS_H

#include <stdint.h>
#include <stdbool.h>

// =====================================================
// MODBUS DEVICE SETTINGS
// =====================================================

// Voltmeter Modbus address
#define MODBUS_SLAVE_ID        2

// Communication settings: 9600, 8N1
#define MODBUS_BAUD_RATE       9600

// Function 04 = Read Input Registers
#define MODBUS_FUNCTION_CODE   0x04

// Read registers 0 to 15
#define MODBUS_START_ADDRESS   0
#define MODBUS_REG_COUNT       16

// Two 16-bit registers form one 32-bit float
#define MODBUS_VALUE_COUNT     8


// =====================================================
// UART / RS485 SETTINGS
// =====================================================

// UART pins internally connected to onboard RS485 IC
#define MODBUS_TX_GPIO         17
#define MODBUS_RX_GPIO         16


// =====================================================
// MEASUREMENT DATA
// =====================================================

typedef struct
{
    uint16_t registers[MODBUS_REG_COUNT];
    float values[MODBUS_VALUE_COUNT];

} modbus_data_t;


// =====================================================
// MODBUS FUNCTIONS
// =====================================================

// Initialize UART and Modbus communication
void modbus_init(void);

// Read Input Registers 0 to 15 from the voltmeter
bool modbus_read_registers(modbus_data_t *data);

// Convert register pairs into 32-bit float values
void modbus_decode_values(modbus_data_t *data);

// Display raw registers and decoded values
void modbus_print_data(const modbus_data_t *data);

#endif