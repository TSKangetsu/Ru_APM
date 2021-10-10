#pragma once
#include <sstream>
#include <iostream>

#define _SYS StringBuilder() << "[RuAPS-SYS] "
#define _APM StringBuilder() << "[RuAPS-APM] "
#define _COM StringBuilder() << "[RuAPS-COM] "
#define _VID StringBuilder() << "[RuAPS-VID] "
#define _EMC StringBuilder() << "[RuAPS-EMC] "

#define STARTUPCOMPLETE "ALL Start Process complete.\n"
#define SIGNALRECVINPUT "System interrupt with:"
#define EXITPROCESSACTD "Terminal request revice, Exiting Procession is Active....\n"
#define SYSTEMEXITEDCAL "System Exiting Procession Complete.\n"
#define APMEXITPROCESSD "APMController Exited.\n"
#define VIDEXITPROCESSD "VIDController Exited.\n"
#define COMEXITPROCESSD "COMController Exited.\n"

//TODO: May we should add some system nofity?
class StringBuilder
{
public:
    template <typename T>
    inline StringBuilder &operator<<(const T &t)
    {
        mStream << t;
        return *this;
    }
    inline std::string get() const
    {
        return mStream.str();
    }
    inline operator std::string() const
    {
        return get();
    }

private:
    std::stringstream mStream;
};

namespace LOGPublicator
{
    void LogPrintSTDIO(std::string Output)
    {
        std::cout << Output;
        std::cout.flush();
    };
};