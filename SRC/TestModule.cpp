#include <iostream>
#include <unistd.h>
#include "APMController/M_APMController.hpp"

int main()
{
	APMMain::APMControl apmtest;
	apmtest.APMRuingSettle();
	return 0;
}