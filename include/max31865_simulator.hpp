#pragma once

#include <inttypes.h>
#include <stddef.h>

struct Max31865Config_t
{
    float *temperatures_data;
    size_t data_length;
    uint16_t time_step_ms;
};

void max31865_simulator_init(Max31865Config_t *config);
uint8_t max31865_simulator_read_register(uint8_t address);
void max31865_simulator_write_register(uint8_t address, uint8_t value);
void max31865_update_simulation();
