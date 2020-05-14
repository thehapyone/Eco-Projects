#include <Arduino.h>

#include <WiFi.h>
#include "mqtt/PubSubClient.h"
// adding library support
#include "sensors/DHT.h"
#include "sensors/OneWire.h"
#include "sensors/DallasTemperature.h"
#include <EEPROM.h>
#include "sensors/GravityTDS.h"
#include "sensors/ecosensor.h"

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
void sensorsSetup(void);
void setup_wifi();
void callback(char* topic, byte* message, unsigned int length);
void reconnect();
// Initialize the EcoSensor library
// Using the TLS2591X
EcoSensor lightsensor(ECO_TLS2591X);

// MQTT Settings here
// Replace the next variables with your SSID/Password combination
const char* ssid = "The Adventurer";
const char* password = "sayiloveyou";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqtt_server = "192.168.1.247";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// End of MQTT config
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  delay(1000);
  sensorsSetup();
  Serial.println("All Sensors Initialized");
}

void sensorsSetup()
{
  // Initialize the sensor
  lightsensor.initialize();
  Serial.print("Light Sensor Initialized\n");
  Serial.print("Get Sensor type : ");
  Serial.print(lightsensor.getSensor());
  //
  dht.begin();
  sensors.begin();	// Start up DS18B20 library
  //////////////////////
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(3.3);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(4096);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization
}

void loop() {

    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    long now = millis();
    if (now - lastMsg > 5000)
    {
      lastMsg = now;
      // covert the sensor value to a char array
      char sensorArray[8];
      dtostrf(Lux, 1, 2, sensorArray);

      // publish the message to the broker
      const char pubTopic[] = "eco/sensors/in";
      client.publish(pubTopic, sensorArray);
    }

    Lux = read_light_sensor();
    Serial.print("Lux = ");
    Serial.print(Lux);
    Serial.print("\r\n");

    /*
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
    */

    delay(1000);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "eco/sensors/out") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
    }
    else if(messageTemp == "off"){
      Serial.println("off");
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("EcoNodeClient")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("eco/sensors/out");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
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
    lux_value = lightsensor.readSensor();
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