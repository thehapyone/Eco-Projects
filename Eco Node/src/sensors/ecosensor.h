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

#define ECO_TLS2591X 1
#define ECO_AM2301 2
#define ECO_DS18B20 3
#define ECO_GRAVITY_TDS 4

#define ECO_ENABLE_DEBUG 0

class EcoSensor
{
    private:
    // some variables
    uint8_t sensorType = 0;
    uint8_t sensorPin = 0;

    void _initializeSensor();

    uint16_t sensorValue = 0;


    public:
    // default constructor
    EcoSensor();
    // Constructor for the Sensor Class
    EcoSensor(uint8_t sensorType);
    EcoSensor(uint8_t sensorType, uint8_t sensorPin);
    // deconstructor. remove and clear the memory if needed
    ~EcoSensor();

    void initialize();
    void setSensor(uint8_t sensor_type);
    uint8_t getSensor();
    int16_t readSensor();

};

#endif


