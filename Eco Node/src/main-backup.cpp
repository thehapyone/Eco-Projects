#include <Arduino.h>

// adding library support
#include "sensors/DEV_Config.h"
#include "sensors/DHT.h"
#include "sensors/OneWire.h"
#include "sensors/DallasTemperature.h"
#include <EEPROM.h>
#include "sensors/GravityTDS.h"
#include "sensors/TSL2591.h"
// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);	
// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

/////////////////////////////////

#define TdsSensorPin 34
GravityTDS gravityTds;

float temperature = 25;
float tdsValue = 0;

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
float ds18b20_temperature(void);
float tds_sensor(float);

void setup() {
  Serial.begin(115200);

  // put your setup code here, to run once:
  // Initialize the sensor
  DEV_ModuleInit();
  Serial.print("Light Sensor Initialized\n");
  TSL2591_Init();
  dht.begin();
  sensors.begin();	// Start up DS18B20 library
  //////////////////////
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(3.3);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(4096);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization

  Serial.println("All Sensors Initialized");
}

void loop() {
    Lux = read_light_sensor();
    Serial.print("Lux = ");
    Serial.print(Lux);
    Serial.print("\r\n");

    my_dht = read_dht(false);
    Serial.println("DHT Results");
    Serial.print(my_dht.humidity);
    Serial.print("% : ");
    Serial.print(my_dht.temperature);
    Serial.println(F("°C/F "));

    Serial.println("DS18B20 Temperature reading");
    float temp = ds18b20_temperature();
    Serial.print(temp);
    Serial.println(F("°C"));   

    tdsValue = tds_sensor(temp);
    Serial.print(tdsValue,0);
    Serial.println("ppm");


    delay(2000);
}


float tds_sensor(float temp1)
{
    //temperature = readTemperature();  //add your temperature sensor and read it
    gravityTds.setTemperature(temp1);  // set the temperature and execute temperature compensation
    gravityTds.update();  //sample and calculate 
    float tdsValue5;
    tdsValue5 = gravityTds.getTdsValue();  // then get the value

    return tdsValue5;
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


float ds18b20_temperature()
{ 
  // Send the command to get temperatures
  sensors.requestTemperatures(); 

  //print the temperature in Celsius
  float temperature = sensors.getTempCByIndex(0);
  return temperature;
}