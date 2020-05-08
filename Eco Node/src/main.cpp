#include <Arduino.h>

// adding library support
#include "sensors/DEV_Config.h"
#include "sensors/TSL2591.h"


// Variables for Light Sensors
UWORD Lux = 0;

void setup() {
  // put your setup code here, to run once:
  // Initialize the sensor
  DEV_ModuleInit();
  Serial.print("Light Sensor Initialized\n");
  TSL2591_Init();
  Serial.println("Started");
}

void loop() {
    Lux = TSL2591_Read_Lux();
    Serial.print("Lux = ");
    Serial.print(Lux);
    Serial.print("\r\n");
    TSL2591_SET_LuxInterrupt(50,200);
    delay(1000);
}
