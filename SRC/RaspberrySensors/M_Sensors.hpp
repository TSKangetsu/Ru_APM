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
#define I2C_ADDR 0x23

namespace sensors
{
    class lightTest
    {
        public:
        lightTest();
    };
}
