#pragma once
#ifndef SENSORS_H
#define SENSORS_H
#endif
#include <iostream>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

namespace Sensors
{
	class I2CSensors_GY67
	{
	public:
		int SensorsFD;
		I2CSensors_GY67();
		int DataGet();

	private:
		int DeviceID = 77;
	};
}