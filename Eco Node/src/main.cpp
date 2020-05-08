#include <Arduino.h>

// adding library support
#include "sensors/DEV_Config.h"
#include "sensors/TSL2591.h"


// Variables for Light Sensors
UWORD Lux = 0;

// Declare all functions
uint16_t read_light_sensor();

void setup() {
  // put your setup code here, to run once:
  // Initialize the sensor
  DEV_ModuleInit();
  Serial.print("Light Sensor Initialized\n");
  TSL2591_Init();
  Serial.println("Started");
}

void loop() {
    Lux = read_light_sensor();
    Serial.print("Lux = ");
    Serial.print(Lux);
    Serial.print("\r\n");
    delay(1000);
}


/*
Function Reads the Light sensor parameter

*/
uint16_t read_light_sensor()
{
    uint16_t lux_value;
    lux_value = TSL2591_Read_Lux();
    return lux_value;
}