
#include "EcoSensors.h"

EcoSensor::EcoSensor()
{

}

EcoSensor::EcoSensor(uint8_t sensorType)
{
    this->sensorType = sensorType;

}

EcoSensor::EcoSensor(uint8_t sensorType, uint8_t sensorPin)
{
    this->sensorType = sensorType;
    this->sensorPin = sensorPin;

    if (this->sensorType == ECO_DS18B20)
    {
        // then create the instance to the OneWire
        // Setup a oneWire instance to communicate with any OneWire device
        //OneWire oneWire(this->sensorPin);	
        this->oneWireDallas.begin(this->sensorPin);
        // Pass oneWire reference to DallasTemperature library
        this->dallasSensors.setOneWire(&this->oneWireDallas);

    }
    
}

EcoSensor::EcoSensor(uint8_t sensorType, uint8_t sensorPin, uint8_t dhtSensorType)
{
    this->sensorType = sensorType;
    this->sensorPin = sensorPin;
    

    if (this->sensorType == ECO_DHT)
    {
        // then create the instance to the DHT
        DHT dhtTemp(sensorPin, dhtSensorType);
        this->dhtSensors = dhtTemp;
    }
    
}

EcoSensor::~EcoSensor()
{
    // the disconstructor.
    // nothing happens here
}

/*
SensorType Values::::
TLS2591X sensor type = 1
AM2301 sensor type = 2
DS18B20 Sensor type = 3
*/


// Functions initialize the set sensor type
// assuming working with TLS2591X sensor type
void EcoSensor::_initializeSensor()
{
    // Initialize the sensor based on the sensor type
    switch (this->sensorType)
    {
    // For TLS2591 Light Sensor
    case ECO_TLS2591X:
        // intialize the sensor
        // Sensor doesn't require setting of pin. It is based on I2C
        DEV_ModuleInit();
        TSL2591_Init();
        if (ECO_ENABLE_DEBUG)
        {
            Serial.println("Light Sensor Initialized successfully");
        }
        break;

    case ECO_DS18B20:
        // intialize the sensor
        this->dallasSensors.begin();
        if (ECO_ENABLE_DEBUG)
        {
            Serial.println("Dallas Temperature Initialized");
        }
        break;

    case ECO_DHT:
        // intialize the sensor
        this->dhtSensors.begin();
        if (ECO_ENABLE_DEBUG)
        {
            Serial.println("DHT Sensor Initialized");
        }
        break;

    default:
        break;
    }

}

void EcoSensor::initialize()
{
    // set the sensor
    // configure the sensor if necessary
    // begin if necessary
    _initializeSensor();
}

void EcoSensor::setSensor(uint8_t sensorType)
{
    this->sensorType = sensorType;
}

uint8_t EcoSensor::getSensor()
{
    return this->sensorType;
}

uint8_t EcoSensor::getSensorPin()
{
    return this->sensorPin;
}

int16_t EcoSensor::readSensor()
{
    // get the latest sensor reading
    switch (this->sensorType)
    {
    case ECO_TLS2591X:
        /* code */
        uint16_t lux_value;
        lux_value = TSL2591_Read_Lux();
        this->sensorValue = lux_value;
        break;

    case ECO_DS18B20:
        /* code */
        this->dallasSensors.requestTemperatures();
        //print the temperature in Celsius
        this->sensorValue = this->dallasSensors.getTempCByIndex(0);

        break;
        
    default:
        break;
        
    }


    return this->sensorValue;
    
}


float EcoSensor::readSensorFloat()
{
    // get the latest sensor reading
    switch (this->sensorType)
    {

    case ECO_DS18B20:
        /* code */
        this->dallasSensors.requestTemperatures();
        //print the temperature in Celsius
        this->sensorValue_float = this->dallasSensors.getTempCByIndex(0);
        if (ECO_ENABLE_DEBUG)
        {
            Serial.print("Sensor readings: ");
            Serial.println(this->sensorValue_float);
        }
        break;
        
    default:
        break;
        
    }

    return this->sensorValue_float;
    
}

multiValues EcoSensor::readSensorAll()
{
    // structure result
    multiValues sensorValues;
    // get the latest sensor reading
    switch (this->sensorType)
    {
    case ECO_DHT:
        // package the results in a structure
        sensorValues.humidity = this->dhtSensors.readHumidity();
        // read in celcius 
        sensorValues.temperature = this->dhtSensors.readTemperature();
        // package the results in a structure

        break;
        
    default:
        break;
        
    }

    return sensorValues;
    
}


void EcoSensor::enableDebug()
{
    // set the debug
    this->ECO_ENABLE_DEBUG = 1;
}

void EcoSensor::disableDebug()
{
    // disable print debug
    this->ECO_ENABLE_DEBUG = 0;
}