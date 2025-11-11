#include "max31865_simulator.hpp"
#include "max31865_registers.hpp"
#include <math.h>
#include <Arduino.h>

typedef struct
{
    float *temperatures_data;
    size_t data_length;
    uint16_t time_step_ms;
} max31865_simulator_config_t;

static max31865_simulator_config_t simulator_config = {0};

static size_t current_index = 0;

static uint32_t last_update_time = 0;

struct Pt100Coefficients_t
{
    const float R0;
    const float A;
    const float B;
    const float C;
    const float RREF;
};

static const Pt100Coefficients_t pt100_coefficients = {
    .R0 = 100.0f, // PT100 base resistance
    .A = 3.9083e-3f,
    .B = -5.775e-7f,
    .C = -4.183e-12f, // only for T < 0°C
    .RREF = 100.0f};

struct Register
{
    uint8_t read_address;
    uint8_t write_address;
    uint8_t value;
};

static Register registers[10] = {
    {0x00, 0x80, 0x00}, // Config register
    {0x01, 0x81, 0x00}, // RTD MSB
    {0x02, 0x82, 0x00}, // RTD LSB
    {0x03, 0x83, 0xFF}, // High fault threshold MSB
    {0x04, 0x84, 0xFF}, // High fault threshold LSB
    {0x05, 0x85, 0x00}, // Low fault threshold MSB
    {0x06, 0x86, 0x00}, // Low fault threshold LSB
    {0x07, 0x87, 0x00}, // Fault status
    // Add more registers as needed
};

static const MAX31865Registers_t max31865_registers = {
    .config_register_read_address = 0x00,
    .config_register_write_address = 0x80,
    .rtd_msb_read_address = 0x01,
    .rtd_lsb_read_address = 0x02,
    .high_fault_threshold_msb_read_address = 0x03,
    .high_fault_threshold_msb_write_address = 0x83,
    .high_fault_threshold_lsb_read_address = 0x04,
    .high_fault_threshold_lsb_write_address = 0x84,
    .low_fault_threshold_msb_read_address = 0x05,
    .low_fault_threshold_msb_write_address = 0x85,
    .low_fault_threshold_lsb_read_address = 0x06,
    .low_fault_threshold_lsb_write_address = 0x86,
    .fault_status = 0x07};

void set_temperature(float temperature);

void max31865_simulator_init(Max31865Config_t *config)
{
    simulator_config.temperatures_data = config->temperatures_data;
    simulator_config.data_length = config->data_length;
    simulator_config.time_step_ms = config->time_step_ms;
}

uint8_t max31865_simulator_read_register(uint8_t address)
{
    size_t length = sizeof(registers) / sizeof(Register);

    for (size_t i = 0; i < length; i++)
    {
        if (registers[i].read_address == address)
        {
            return registers[i].value;
        }
    }
    return 0xFF; // Default value if register not found
}

void max31865_simulator_write_register(uint8_t address, uint8_t value)
{
    size_t length = sizeof(registers) / sizeof(Register);

    for (size_t i = 0; i < length; i++)
    {
        if (registers[i].write_address == address)
        {
            registers[i].value = value;
            return;
        }
    }
}

void max31865_update_simulation()
{
    uint32_t current_time = millis();
    if (current_time - last_update_time < simulator_config.time_step_ms)
    {
        return; // Not time to update yet
    }
    last_update_time = current_time;

    if (simulator_config.data_length == 0)
        return;

    float current_temperature = simulator_config.temperatures_data[current_index];
    set_temperature(current_temperature);

    current_index = (current_index + 1) % simulator_config.data_length;
}

void set_temperature(float temperature)
{
    float R0 = pt100_coefficients.R0;
    float A = pt100_coefficients.A;
    float B = pt100_coefficients.B;
    float C = pt100_coefficients.C;
    float RREF = pt100_coefficients.RREF;

    float resistance;
    if (temperature >= 0)
    {
        resistance = R0 * (1 + A * temperature + B * temperature * temperature);
    }
    else
    {
        resistance = R0 * (1 + A * temperature + B * temperature * temperature + C * (temperature - 100) * temperature * temperature * temperature);
    }

    uint16_t rtd_value = (uint16_t)((resistance / RREF) * 32768.0f);
    rtd_value <<= 1; // Shift left to align with MAX31865 format

    // Update RTD MSB and LSB registers
    for (size_t i = 0; i < sizeof(registers) / sizeof(Register); i++)
    {
        if (registers[i].read_address == max31865_registers.rtd_msb_read_address)
        {
            registers[i].value = (rtd_value >> 8) & 0xFF;
        }
        else if (registers[i].read_address == max31865_registers.rtd_lsb_read_address)
        {
            registers[i].value = rtd_value & 0xFF;
        }
    }
}
