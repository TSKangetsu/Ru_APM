#pragma once
#ifndef MRECSETTER_H
#define MRECSETTER_H
#endif

#include <iostream>

#ifdef linux
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "T_PCA9685.h"
#endif

#ifdef linux
namespace I2Ccontrol_REC
{
	class PCA9685
	{
		
	};
}
#endif