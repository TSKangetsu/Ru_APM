#include "SensorsDevice.hpp"

Sensors::I2CSensors_GY67::I2CSensors_GY67()
{
	SensorsFD = wiringPiI2CSetup(DeviceID);

	wiringPiI2CWrite(SensorsFD, 0x1E);
}

int Sensors::I2CSensors_GY67::DataGet()
{
	while (true)
	{
		sleep(1);
		std::cout << wiringPiI2CReadReg8(DeviceID, 0xA4);
	}
}