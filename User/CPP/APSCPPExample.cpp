#include "APSUser.hpp"

void APSCPPUser::Setup()
{
    clockTimer = 0;
    RequestDISARM(false);
};

void APSCPPUser::Loop()
{
    GetRTStatus(Status);
    GetRCValues(RCValues);
    if (1700 < RCValues[6] && RCValues[6] < 2000)
    {
        clockTimer++;
        //Misson Test
        if (clockTimer == 500)
        {
            TakeoffFlag = true;
            MissonASide = true;
        }

        if (MissonASide == true)
        {
            if ((47 < Status[RTStatus_Movement_Z] && Status[RTStatus_Movement_Z] < 53) &&
                (-5 < Status[RTStatus_Movement_Y] && Status[RTStatus_Movement_Y] < 5))
            {
                SetUserPosition(0, -170, 85, false);
            }
            if ((-173 < Status[RTStatus_Movement_Y] && Status[RTStatus_Movement_Y] < -167))
            {
                LandingFlag = true;
                MissonASide = false;
            }
        }
        //Timer Test
        // if (clockTimer == 200)
        // {
        //     TakeoffFlag = true;
        // }
        // if (clockTimer == 1000)
        // {
        //     LandingFlag = true;
        // }
        //TakeOff and Landing
        {
            if (TakeoffFlag)
            {
                RequestDISARM(true);
                TakeoffFlag = false;
            }
            if (LandingFlag)
            {
                SetUserSpeed(0, 0, 35);
                if (Status[RTStatus_Movement_Z] < 5)
                {
                    RequestDISARM(false);
                    LandingFlag = false;
                }
            }
        }
    }
    else
    {
        clockTimer = 0;
        TakeoffFlag = false;
        LandingFlag = false;
        RequestDISARM(false);

        MissonASide = false;
        MissonBSide = false;
        MissonCSide = false;
    }
};