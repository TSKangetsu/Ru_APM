#include "APSUser.hpp"

void UserAPICPP::UserSetup()
{
    MainCap.open(0);
    IOStream = std::thread(
        [&]
        {
            while (true)
            {
                std::cout << "ARM:" << IsARM << "\n";
                std::cin >> ChannelARMData;
            }
        });
};

void UserAPICPP::UserLoop()
{
    int Chan[] = {1495, 1495, 997, 1495, ChannelARMData, 997, 0, 0, 0, 0};
    FakeRCImplment(Chan, false);
    GetRTStatus(RTStatus);
    IsARM = RTStatus[RTStatus_IsARM];
};