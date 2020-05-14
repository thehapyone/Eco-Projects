
#include "ecosensor.h"

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

int16_t EcoSensor::readSensor()
{
    // get the latest sensor reading
    switch (this->sensorType)
    {
    case ECO_TLS2591X:
        /* code */
        uint8_t lux_value;
        lux_value = TSL2591_Read_Lux();
        this->sensorValue = lux_value;
        break;
    
    default:
        break;
        
    }

    // ideally, it should return a struct or something similar to handle multiple values.
    return this->sensorValue;
}
