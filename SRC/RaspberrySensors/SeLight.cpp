#include "M_Sensors.hpp"

I2CSensors::SeLight::SeLight(int I2CADDR)
{
    DEVICEADDR = I2CADDR;
}

// I2CSensors::SeLight::SeLight(int I2C_ADDR)
// {
//     char buf[3];
//     char val;
//     float flight;
//     int i2c_light_dump = open("/dev/i2c-1", O_RDWR);
//     if (i2c_light_dump >= 0)
//     {

//         if (ioctl(i2c_light_dump, I2C_SLAVE, I2C_ADDR) >= 0)
//         {
//         }
//         else
//         {
//             std::cout << "\033[31m[I2CSensors] initialization io faild (selight) \033[0m\n";
//         }
//     }
//     else
//     {
//         std::cout << "\033[31m[I2CSensors] Failed to open the i2cdump (Selight)\033[0m\n";
//     }

//     val = 0x01;
//     if (write(i2c_light_dump, &val, 1) < 0)
//     {
//         printf("write 0x01 err\r\n");
//     }
//     val = 0x10;
//     if (write(i2c_light_dump, &val, 1) < 0)
//     {
//         printf("write 0x10 err\r\n");
//     }
//     while (1)
//     {
//         if (read(i2c_light_dump, &buf, 3))
//         {
//             flight = (buf[0] * 256 + buf[1]) / 1.2;
//             printf("light is %6.3f\r\n", flight);
//         }
//         else
//         {
//             printf("read light error\r\n");
//         }
//         usleep(100000);
//     }
// }
