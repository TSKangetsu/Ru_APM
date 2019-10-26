#pragma once
#ifndef MRECSETTER_H
#define MRECSETTER_H
#endif

#include <iostream>

#ifdef linux
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <unistd.h>
#include "T_PCA9685.h"
#endif

#ifdef linux
namespace I2Ccontrol_REC
{
	class flight_control_connector
	{
	public:
		int PCA9658_Address = 0x40;
		int PCA9658_PinBase = 65;
		int PCA9658_fd;
		struct PWM_State
		{
			int APWM_Freq = 67.5;
			bool PWM_STARTED;
			int Free_Throttle;

			int PWM_Throttle_PIN = 2;
			int PWM_Throttle;
			int PWM_Throttle_Free = 270;
			int PWM_Throttle_StartUP = 270;

			int PWM_Course_PIN = 3;
			int PWM_Course;
			int PWM_Course_Free;
			int PWM_Course__StartUP = 600;

			int PWM_Front_Subduction_PIN = 1;
			int PWM_Front_Subduction;
			int PWM_Front_Subduction__StartUP = 600;

			int PWM_Rear_Subduction_PIN = 0;
			int PWM_Rear_Subduction;
			int PWM_Rear_Subduction_StartUP = 270;
		} PWM_State;

		flight_control_connector();
		void MotorStartUP();
		void MotorStop();
	};

	class flight_Control_Input
	{

	};
}
#endif