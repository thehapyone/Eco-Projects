#include <Arduino.h>

#include <WiFi.h>
#include "mqtt/PubSubClient.h"
// adding library support

#include "sensors/EcoSensors.h"


////////////////////////////////////////////////////////////////////////
#define waterSensorPin 4
#define TdsSensorPin 34
#define DHTPIN 5     // Digital pin connected to the DHT sensor


////////////////////////////////////////////////////////////////////////

GravityTDS gravityTds;

float temperature = 25;
float tdsValue = 0;

// Variables for Light Sensors
UWORD Lux = 0;

// Declare all functions
uint16_t read_light_sensor();
multiValues read_dht();
float ds18b20_temperature(void);
float tds_sensor(float);
void sensorsSetup(void);
void setup_wifi();
void callback(char* topic, byte* message, unsigned int length);
void reconnect();
// Initialize the EcoSensor library
// Using the TLS2591X
EcoSensor lightsensor(ECO_TLS2591X);

// For the DS18B20 Sensor
EcoSensor waterTempSensor(ECO_DS18B20, waterSensorPin);

// For the DHT Sensor
EcoSensor dhtMultiSensor(ECO_DHT, DHTPIN, AM2301);

// Hold the results for the DHT sensors
multiValues my_dht;

// For TDS Sensor
EcoSensor gravitySensor(ECO_GRAVITY_TDS, TdsSensorPin);

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

  // enable debug
  waterTempSensor.enableDebug();
  // Initialize DS18b20 sensor
  waterTempSensor.initialize();


  // DHT Sensor
  dhtMultiSensor.initialize();
  //////////////////////
  gravitySensor.initialize();
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

    
    my_dht = read_dht();
    Serial.println("DHT Results");
    Serial.print(my_dht.humidity);
    Serial.print("% : ");
    Serial.print(my_dht.temperature);
    Serial.println(F("°C/F "));

    Serial.println("DS18B20 Temperature reading");
    float temp = ds18b20_temperature();
    Serial.print(temp);
    Serial.println(F("°C")); 


    /*

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
    gravitySensor.setParameter(ECO_GRAVITY_TDS, temp1);  // set the temperature and execute temperature compensation
    float tdsValue5;
    tdsValue5 = gravitySensor.readSensorFloat();  // then get the value

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


multiValues read_dht() {

  multiValues output = dhtMultiSensor.readSensorAll();

  // Check if any reads failed and exit early (to try again).
  if (isnan(output.humidity) || isnan(output.temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
  }

  return output;
}


float ds18b20_temperature()
{ 
  //print the temperature in Celsius
  float temp = waterTempSensor.readSensorFloat();
  return temp;
}