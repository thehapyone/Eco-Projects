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

float temperature = 25;
float tdsValue = 0;

// Variables for Light Sensors
UWORD Lux = 0;

// Declare all functions
uint16_t read_light_sensor(void);
multiValues read_dht(void);
float ds18b20_temperature(void);
float tds_sensor(float);
void sensorsSetup(void);
void setup_wifi(void);
void callback(char* topic, byte* message, unsigned int length);
void reconnect(void);
String takeReadings(void);
void publishData(void);

// Initialize the EcoSensor library
// Using the TLS2591X
EcoSensor lightsensor(ECO_TLS2591X);

// For the DS18B20 Sensor
EcoSensor waterTempSensor(ECO_DS18B20, waterSensorPin);

// For the DHT Sensor
EcoSensor dhtMultiSensor(ECO_DHT, DHTPIN, AM2301);

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

// Time to send data
long time_to_send = 5; // in minutes
// publish topic
const char pubTopic[] = "eco/sensors/in";
const char subTopic[] = "eco/sensors/out";

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


void loop() {

    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    publishData();
    // easssssy do nothing here
    delay(10);
}

void sensorsSetup()
{
  // Initialize the sensor
  lightsensor.initialize();

  // Initialize DS18b20 sensor
  waterTempSensor.initialize();

  // DHT Sensor
  dhtMultiSensor.initialize();
  //////////////////////
  gravitySensor.initialize();
}

String takeReadings(void)
{
  // lux
  Lux = read_light_sensor();
  // dht
  // Hold the results for the DHT sensors
  multiValues my_dht;
  my_dht = read_dht();
  // ds18b20
  float ds_temp = ds18b20_temperature();
  // tds
  tdsValue = tds_sensor(ds_temp);
  // format result to string
  String output = String(Lux)+","+String(my_dht.humidity)+","+String(my_dht.temperature)+","+String(ds_temp)+","+String(tdsValue);
  return output;
}

void publishData(void)
{
  long now = millis();

  long time_to_millis = time_to_send * 60 * 1000;

  if (now - lastMsg > time_to_millis)
  {
    lastMsg = now;
    String results = takeReadings();

    uint16_t n = results.length();

    // covert the sensor value to a char array
    char sensorArray[n + 1];
    // copy contents to the char array
    strcpy(sensorArray, results.c_str());
    // publish the message to the broker
    client.publish(pubTopic, sensorArray);
    Serial.print("Published readings: ");
    Serial.println(results);
  }

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
      client.subscribe(subTopic);
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