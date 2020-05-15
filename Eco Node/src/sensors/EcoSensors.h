/*
*   This is a Unified Sensor API for the EcoSense Platform
*   It allows workind directly with various set of sensors both digital and analog
*
*   The goal is to make this library raw C++ as much as possible.
*   Created on: May 13th, 2020
*   Author: Ayo Ayibiowu
*   Email: charlesayibiowu@hotmail.com
*   Version: v0.1
*/

#pragma once
#ifndef ECOSENSOR_H
#define ECOSENSOR_H

#include <stdint.h>
#include "DEV_Config.h"
#include "TSL2591.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "DHT.h"
#include <EEPROM.h>
#include "sensors/GravityTDS.h"

#define ECO_TLS2591X 1
#define ECO_DHT 2
#define ECO_DS18B20 3
#define ECO_GRAVITY_TDS 4

/* 
Structure For holding Sensors with multiple values
*/
struct multiValues
{
  /* data */
  float temperature = 0;
  float humidity = 0;
};

class EcoSensor
{
    private:
    // some variables
    uint8_t sensorType = 0;
    uint8_t sensorPin = 0;

    // variabls for dallas sensors
    OneWire oneWireDallas;
    DallasTemperature dallasSensors;
    // Variable for DHT sensor
    DHT dhtSensors;

    uint8_t ECO_ENABLE_DEBUG = 0;

    void _initializeSensor();

    // for integer base values
    int32_t sensorValue = 0;
    // for floating base values
    float sensorValue_float = 0.0;


    public:
    // default constructor
    EcoSensor();
    // Constructor for the Sensor Class
    EcoSensor(uint8_t sensorType);
    EcoSensor(uint8_t sensorType, uint8_t sensorPin);
    EcoSensor(uint8_t sensorType, uint8_t sensorPin, uint8_t dhtSensorType); // For DHT Sensor type

    // deconstructor. remove and clear the memory if needed
    ~EcoSensor();

    void initialize();
    void setSensor(uint8_t sensor_type);
    uint8_t getSensor();
    uint8_t getSensorPin();
    // single value read
    int16_t readSensor();
    // single value float read
    float readSensorFloat();
    // multiple value read
    multiValues readSensorAll();

    // debug information
    void enableDebug(void);
    void disableDebug(void);

};

#endif


