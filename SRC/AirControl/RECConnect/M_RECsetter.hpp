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
	class PCA9685_AirSet
	{
	public:
		int PCA9658_Address = 0x40;
		int PCA9658_PinBase = 65;
		int PCA9658_fd;
		struct PWM_State
		{
			int APWM_Freq = 67.5;

			int PWM_Throttle_PIN = 0;
			int PWM_Throttle;
			int PWM_Throttle_StartUP = 300;

			int PWM_Course_PIN = 1;
			int PWM_Course;
			int PWM_Course__StartUP = 600;

			int PWM_Front_Subduction_PIN = 2;
			int PWM_Front_Subduction;
			int PWM_Front_Subduction__StartUP = 600;

			int PWM_Rear_Subduction_PIN = 3;
			int PWM_Rear_Subduction;
			int PWM_Rear_Subduction_StartUP = 300;
		} PWM_State;

		PCA9685_AirSet();
	};
}
#endif