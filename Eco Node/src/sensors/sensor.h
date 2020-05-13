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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


class EcoSensor
{
    private:


    public:
    EcoSensor();
    void initialize(int sensor_type);
    void setSensor(uint8_t sensor_type);




}

#endif


