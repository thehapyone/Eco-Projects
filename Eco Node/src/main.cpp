#include <Arduino.h>

// adding library support
#include "sensors/DEV_Config.h"
#include "sensors/TSL2591.h"
#include "sensors/DHT.h"

// Variables for Light Sensors
UWORD Lux = 0;

// Variables for DHT Sensors
#define DHTPIN 5     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT21   // DHT 21 (AM2301)
// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

struct dht_results
{
  /* data */
  float temperature;
  float humidity;
};

dht_results my_dht = {0, 0};

// Declare all functions
uint16_t read_light_sensor();
dht_results read_dht(bool);

void setup() {
  // put your setup code here, to run once:
  // Initialize the sensor
  Serial.begin(115200);
  //DEV_ModuleInit();
  Serial.print("Light Sensor Initialized\n");
  //TSL2591_Init();
  dht.begin();
  Serial.println("All Sensors Initialized");
}

void loop() {
    //Lux = read_light_sensor();
    Serial.print("Lux = ");
    Serial.print(Lux);
    Serial.print("\r\n");
    my_dht = read_dht(false);
    Serial.println("DHT Results");
    Serial.print(my_dht.humidity);
    Serial.print(" : ");
    Serial.print(my_dht.temperature);

    delay(2000);
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


dht_results read_dht(bool converter) {

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = 0;

  dht_results output = {0, 0};
  
  if (converter == true)
  {
    // Read temperature as Fahrenheit (isFahrenheit = true)
    t = dht.readTemperature(true);
  }
  else 
  {
    // Read temperature as Celsius (the default)
    t = dht.readTemperature();
  }

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return output;
  }
  output.humidity = h;
  output.temperature = t;

  return output;
}