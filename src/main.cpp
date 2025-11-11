#include <Arduino.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include "inttypes.h"
#include <math.h>

#include "max31865_simulator.hpp"

volatile uint8_t receivedData;
volatile bool dataReady = false;

static float temperatures[] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f,
                                     11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f,
                                     20.0f, 21.0f, 22.0f, 23.0f, 24.0f, 25.0f, 26.0f, 27.0f, 28.0f, 29.0f,
                                     30.0f, 31.0f, 32.0f, 33.0f, 34.0f, 35.0f, 36.0f, 37.0f, 38.0f, 39.0f,
                                     40.0f, 41.0f, 42.0f, 43.0f, 44.0f, 45.0f, 46.0f, 47.0f, 48.0f, 49.0f};

static Max31865Config_t default_simulator_config = {
    .temperatures_data = temperatures,
    .data_length = 50,
    .time_step_ms = 500,
};

void setup()
{
  pinMode(MISO, OUTPUT); // Slave sends data via MISO
  pinMode(SS, INPUT);    // SS controlled by master
  pinMode(MOSI, INPUT);
  pinMode(SCK, INPUT);

  // Enable SPI in Slave mode, and enable interrupt
  SPCR = _BV(SPE) | _BV(SPIE) | _BV(CPHA);

  Serial.begin(9600);
  sei(); // Enable global interrupts

  max31865_simulator_init(&default_simulator_config); // Initialize simulator with default config
}

ISR(SPI_STC_vect)
{
  receivedData = SPDR; // Read data register
  dataReady = true;
  SPDR = max31865_simulator_read_register(receivedData); // Load response into data register
}

void loop()
{
  if (dataReady)
  {
    Serial.print("Received: ");
    Serial.println((char)receivedData);
    dataReady = false;
  }
}
