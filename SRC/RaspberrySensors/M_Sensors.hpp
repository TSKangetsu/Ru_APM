#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ioctl.h>
#include <iostream>
#include <nlohmann/json.hpp>

namespace I2CSensors
{
class SeLight
{
public:
    SeLight(int I2C_ADDR);

private:
    int DEVICEADDR;
};
} // namespace I2CSensors
