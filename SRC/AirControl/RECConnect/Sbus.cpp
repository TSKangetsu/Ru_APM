#include "Sbus.hpp"

_SBUS::_SBUS::_SBUS(int wpin)
{
	this->wpin = wpin;
}

void _SBUS::_SBUS::FrameWait()
{
	wiringPiSetup();
	pinMode(wpin, OUTPUT);
	wiringPiISR(wpin,
		INT_EDGE_FALLING,
		&this->FrameRecive);
}

void _SBUS::_SBUS::FrameRecive()
{
	for (int count = 0; count < 50; count++)
	{
		int SingleData = digitalReadByte();
		std::cout << SingleData << "\n";
	}
}