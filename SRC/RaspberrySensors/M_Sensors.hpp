#pragma once
#ifndef MSENSORS_H
#define MSENSORS_H
#endif

#include "../ToolsInclude/M_Tools.hpp"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <iostream>
#include <fstream>

#ifdef linux
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#endif

#ifdef linux
namespace Sensors
{
class SeLight
{
public:
	SeLight(int I2C_ADDR);

private:
	int DEVICEADDR;
};
}
#endif