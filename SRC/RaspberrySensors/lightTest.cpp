#include "M_Sensors.hpp"

sensors::lightTest::lightTest()
{
        int fd;
        char buf[3];
        char val,value;
        float flight;
        fd=open("/dev/i2c-1",O_RDWR);
        if(fd<0)
        {
                printf("err open file:%s\r\n",strerror(errno));
        }
        if(ioctl( fd,I2C_SLAVE,I2C_ADDR)<0 )
        {
                printf("ioctl error : %s\r\n",strerror(errno));
        }
        val=0x01;
        if(write(fd,&val,1)<0)
        {
                printf("write 0x01 err\r\n");
        }
        val=0x10;
        if(write(fd,&val,1)<0)
        {
                printf("write 0x10 err\r\n");
        }
        while(1)
        {
                if(read(fd,&buf,3))
                {
                        flight=(buf[0]*256+buf[1])/1.2;
                        printf("light is %6.3f\r\n",flight);
                }
                else
                {
                        printf("read light error\r\n");
                }
                usleep(100000);
        }
}