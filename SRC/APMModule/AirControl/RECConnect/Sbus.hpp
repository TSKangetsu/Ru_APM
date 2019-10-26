#pragma once
#ifndef SBUS_H
#define SBUS_H
#endif
#include <wiringPi.h>
#include <iostream>
#include <unistd.h>

namespace _SBUS
{
	class _SBUS
	{
	public:
		int wpin;
		_SBUS(int wpin);
		void FrameWait();
	private:
		//int SingleData;
		void FrameRecive();
	};
}