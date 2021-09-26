#include "_Mainfunction/SchedulerManeger.hpp"

void SignalCatcher(int Signal)
{
    RuAPSSys::APSSystemSignal = Signal;
}

int main(int argc, char **argv)
{
    RuAPSSys::SchedulerController().SystemMonitorReg();
    //  Reg Signal Catch
    std::signal(SIGINT, SignalCatcher);
    std::signal(SIGTERM, SignalCatcher);
    //  Wait untill std::terminal()
    usleep(-1);
}
