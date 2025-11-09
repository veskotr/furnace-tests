#include <Arduino.h>

#include <avr/io.h>
#include <avr/interrupt.h>

volatile byte receivedData;
volatile bool dataReady = false;

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
}

ISR(SPI_STC_vect)
{
  receivedData = SPDR; // Read data register
  dataReady = true;
  SPDR = receivedData; // Echo back the received data
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
