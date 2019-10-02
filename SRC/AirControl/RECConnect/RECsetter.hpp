#pragma once
#ifndef MRECSETTER_H
#define MRECSETTER_H
#endif

#include <iostream>

#ifdef linux
#include <wiringPi.h>
#include <wiringPiI2C.h>
#endif

namespace I2Ccontrol_REC
{
	class PCA9685
	{
		static bool PCA9650Setter() {};
	};
}