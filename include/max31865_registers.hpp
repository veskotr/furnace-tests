#pragma once

#include <inttypes.h>

struct MAX31865Registers_t
{
    const uint8_t config_register_read_address;
    const uint8_t config_register_write_address;
    const uint8_t rtd_msb_read_address;
    const uint8_t rtd_lsb_read_address;
    const uint8_t high_fault_threshold_msb_read_address;
    const uint8_t high_fault_threshold_msb_write_address;
    const uint8_t high_fault_threshold_lsb_read_address;
    const uint8_t high_fault_threshold_lsb_write_address;
    const uint8_t low_fault_threshold_msb_read_address;
    const uint8_t low_fault_threshold_msb_write_address;
    const uint8_t low_fault_threshold_lsb_read_address;
    const uint8_t low_fault_threshold_lsb_write_address;
    const uint8_t fault_status;
};

extern const MAX31865Registers_t max31865_registers;