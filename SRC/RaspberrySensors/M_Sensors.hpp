#pragma once
#include "../ToolsInclude/M_Tools.hpp"
#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <sys/ioctl.h>
#include <iostream>
#include <fstream>
#include "../_thirdparty/json/single_include/nlohmann/json.hpp"

namespace Sensors
{
class SeLight
{
public:
	SeLight(int I2C_ADDR);

private:
	int DEVICEADDR;
};
} // namespace Sensors
