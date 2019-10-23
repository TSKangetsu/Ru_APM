#pragma once
#ifndef APMSYSTEM_H
#define APMSYSTEM_H
#endif
#include <iostream>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringSerial.h>
namespace APMSystem
{
	static float PWM_Multiple = 40;

	static float _Roll_ISet = 5;
	static float _uORB_Roll_Output_P;
	static float _uORB_Roll_Input_P;
	static float _Roll_I;
	static float _Roll_D;
	static float _Pitch_ISet = 5;
	static float _uORB_Pitch_Output_P;
	static float _uORB_Pitch_Input_p;
	static float _Pitch_I;
	static float _Pitch_D;
	static float _Yall_ISet = 5;
	static float _uORB_Yall_Output_P;
	static float _uORB_Yall_Input_p;
	static float _Yall_I;
	static float _Yall_D;

	static float _uORB_MPU9250_Input_Roll;
	static float _uORB_MPU9250_Input_Roll_Diff;
	static float _uORB_MPU9250_Input_Pitch;
	static float _uORB_MPU9250_Input_Pitch_Diff;
	static float _uORB_MPU9250_Input_Yall;
	static float _uORB_MPU9250_Input_Yall_Diff;
	static float PWM_Current[4];

	class FeedBackSystem
	{
	public:
		void MPU9250Settle();
		void MS5611Settle();
	};

	class RCController_Input
	{

	};

	class PIDCaculator
	{
	public:
		float PID_Operator(float DataSet, float DataDiff, float DataCurrent);
	private:
		float I_OPerator();
		float D_OPerator();
	};

	class ControllerMixer
	{

	};

	class MotorController
	{

	};
}