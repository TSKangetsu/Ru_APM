#include <queue>
#include <thread>
#include <fstream>
#include <dirent.h>
#include <wiringPi.h>
#include <opencv2/opencv.hpp>
#include "../User/CPP/APSUser.hpp"
#include "_Excutable/Drive_Json.hpp"
#include "RPiSingleAPM/src/SingleAPM.hpp"
#include "_VisionBase/VisionBaseExec.hpp"
#include "_VisionBase/VideoStreamDrive/Drive_RTMPPush.hpp"
#include "_VisionBase/CameraDrive/Drive_V4L2Reader.hpp"
#include "MessageController/MessageController.hpp"
#include "LuaLocal.hpp"

using json = nlohmann::json;
using namespace SingleAPMAPI;

#define Server_WebSocket 0
#define QueueBufferSize 2

namespace Action
{
    class ConfigCenter
    {
    protected:
        struct VideoConfig
        {
            bool EnableVideoControll = false;
            bool EnableVideoStream = false;
        } VC;
        struct MessageConfig
        {
            int ServerType = Server_WebSocket;
        } MC;
        struct ControllerConfig
        {
            APMSettinngs setting;
            bool CPPAllowUserScript = true;
            bool LUAAllowUserScript = false;
            bool EnableControllerDEBUG = true;
        } CC;
    };

    class Action : private ConfigCenter, private RPiSingleAPM, private LuaLocal
    {
    public:
        Action()
        {
            wiringPiSetup();
            //Controller Init
            {
                configAPMSettle("/etc/APMconfig.json", CC.setting);
                RPiSingleAPMInit(CC.setting);
                IMUSensorsTaskReg();
                ControllerTaskReg();
                AltholdSensorsTaskReg();
                PositionTaskReg();
                ESCUpdateTaskReg();
                ControllerThread = std::thread(
                    [&]
                    {
                        while (true)
                        {
                            if (dataQueue.size() > QueueBufferSize)
                                dataQueue.pop();
                            json OutputJSON = {
                                {"type", 4200},
                                {"flyMode", MessageController::StringRounder(AF.AutoPilotMode, 2)},
                                {"ESCA1", MessageController::StringRounder(EF._uORB_A1_Speed, 2)},
                                {"ESCA2", MessageController::StringRounder(EF._uORB_A2_Speed, 2)},
                                {"ESCB1", MessageController::StringRounder(EF._uORB_B1_Speed, 2)},
                                {"ESCB2", MessageController::StringRounder(EF._uORB_B2_Speed, 2)},
                                {"ESDBETA", MessageController::StringRounder(EF._uORB_ESC_MIX_Range, 2)},
                                {"IDYNAMIC", MessageController::StringRounder(PF._uORB_PID_I_Dynamic_Gain, 2)},
                                {"LevelRoll", MessageController::StringRounder(PF._uORB_Leveling__Roll, 2)},
                                {"LevelPitch", MessageController::StringRounder(PF._uORB_Leveling_Pitch, 2)},
                                {"LevelYaw", MessageController::StringRounder(PF._uORB_Leveling___Yaw, 2)},

                                {"GRYOPitch", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Gryo_Pitch, 2)},
                                {"GRYORoll", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Gryo__Roll, 2)},
                                {"GRYOYaw", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Gryo___Yaw, 2)},
                                {"AccelPitch", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Accel_Pitch, 2)},
                                {"AccelRoll", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Accel__Roll, 2)},
                                {"RealPitch", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Real_Pitch, 2)},
                                {"RealRoll", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Real__Roll, 2)},
                                {"PIDPitch", MessageController::StringRounder(PF._uORB_Leveling_Pitch, 2)},
                                {"PIDRoll", MessageController::StringRounder(PF._uORB_Leveling__Roll, 2)},

                                {"FastPressure", MessageController::StringRounder(SF._Tmp_MS5611_PressureFast, 2)},
                                {"FinalPressure", MessageController::StringRounder(SF._uORB_MS5611_PressureFinal, 2)},
                                {"HoldPressure", MessageController::StringRounder(PF._uORB_PID_AltHold_Target, 2)},
                                {"HoldThrottle", MessageController::StringRounder(PF._uORB_PID_Alt_Throttle, 2)},

                                {"GPSSatCount", MessageController::StringRounder(SF._uORB_GPS_Data.satillitesCount, 2)},

                                {"RCRoll", MessageController::StringRounder(RF._uORB_RC_Out__Roll, 2)},
                                {"RCPitch", MessageController::StringRounder(RF._uORB_RC_Out_Pitch, 2)},
                                {"RCThrottle", MessageController::StringRounder(RF._uORB_RC_Out_Throttle, 2)},
                                {"RCYaw", MessageController::StringRounder(RF._uORB_RC_Out___Yaw, 2)},
                                {"RCYaw", MessageController::StringRounder(RF._uORB_RC_Out___Yaw, 2)},

                                {"RawADFX", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_MPU9250_ADF_X, 2)},
                                {"RawADFY", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_MPU9250_ADF_Y, 2)},
                                {"RawADFZ", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_MPU9250_ADF_Z, 2)},
                                {"RawSAX", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_MPU9250_A_Static_X, 2)},
                                {"RawSAY", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_MPU9250_A_Static_Y, 2)},
                                {"RawSAZ", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_MPU9250_A_Static_Z, 2)},

                                {"SpeedX", MessageController::StringRounder((int)SF._uORB_True_Speed_X, 2)},
                                {"SpeedY", MessageController::StringRounder((int)SF._uORB_True_Speed_Y, 2)},
                                {"FlowSpeedX", MessageController::StringRounder((int)SF._uORB_Flow_Speed_X, 2)},
                                {"FlowSpeedY", MessageController::StringRounder((int)SF._uORB_Flow_Speed_Y, 2)},

                                {"FlowOutX", MessageController::StringRounder((int)SF._uORB_Flow_Filter_XOutput, 2)},
                                {"FlowOutY", MessageController::StringRounder((int)SF._uORB_Flow_Filter_YOutput, 2)},
                                {"FlowOutXAsix", MessageController::StringRounder((int)SF._uORB_Flow_Body_Asix_X, 2)},
                                {"FlowOutYAsix", MessageController::StringRounder((int)SF._uORB_Flow_Body_Asix_Y, 2)},

                                {"AccelX", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Acceleration_X, 2)},
                                {"AccelY", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Acceleration_Y, 2)},
                                {"AccelZ", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Acceleration_Z, 2)},
                                {"VIBEX", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Accel_VIBE_X, 2)},
                                {"VIBEY", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Accel_VIBE_Y, 2)},
                                {"VIBEZ", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Accel_VIBE_Z, 2)},

                                {"MovementX", MessageController::StringRounder(SF._uORB_True_Movement_X, 2)},
                                {"MovementY", MessageController::StringRounder(SF._uORB_True_Movement_Y, 2)},

                                {"SpeedZ", MessageController::StringRounder(SF._uORB_True_Speed_Z, 2)},
                                {"SonarClimbeRate", MessageController::StringRounder(SF._uORB_Flow_ClimbeRate, 2)},
                                {"ClimbeRate", MessageController::StringRounder(SF._uORB_MS5611_ClimbeRate, 2)},
                                {"EKFClimbeRate", MessageController::StringRounder(PF._uORB_PID_SpeedZ_Final, 2)},

                                {"MovementZ", MessageController::StringRounder(SF._uORB_True_Movement_Z, 2)},
                                {"TargetAlttitude", MessageController::StringRounder(PF._uORB_PID_AltHold_Target, 2)},
                                {"PressureAlttitude", MessageController::StringRounder(PF._uORB_PID_MS5611_AltInput, 2)},
                                {"SonarAltitude", MessageController::StringRounder(PF._uORB_PID_Sonar_AltInput, 2)},
                                {"EKFAltitude", MessageController::StringRounder(PF._uORB_PID_AltInput_Final, 2)},
                                {"AltThrottle", MessageController::StringRounder(PF._uORB_PID_Alt_Throttle, 2)},
                                //
                                {"GPSNMode:", SF._uORB_GPS_Data.lat_North_Mode},
                                {"GPSEMode:", SF._uORB_GPS_Data.lat_East_Mode},
                                {"F_ESCARM", AF._flag_ESC_ARMED},
                                {"F_Error", AF._flag_Error},
                                {"F_RCDisconnect", AF._flag_RC_Disconnected},
                                {"F_GPSDisconnect", AF._flag_GPS_Disconnected},
                            };
                            dataBuffer[0] = OutputJSON.dump();
                            dataQueue.push(MessageController::dataCreator(255, dataBuffer, 2));
                            SaftyCheckTaskReg();
                            usleep(20000);
                        }
                    });
            }

            if (MC.ServerType == Server_WebSocket)
            {
                MessageServer.WebSocketServerInit().OnConnection(
                                                       [&](auto *req, auto *data)
                                                       {
                                                           std::string output = "4000";
                                                           MessageController::WebSocketServer::dataSender(req, output, WCT_TXTDATA);
                                                       })
                    .OnMessage(
                        [&](auto *req, auto *data, auto opcode, auto len)
                        {
                            std::string output;
                            if (strncmp(data, "4010", 4) == 0)
                            {
                                output = "4110";
                                MessageController::WebSocketServer::dataSender(req, output, WCT_TXTDATA);
                            }
                            if (strncmp(data, "4110", 4) == 0)
                            {
                                output = "4112";
                                MessageController::WebSocketServer::dataSender(req, output, WCT_TXTDATA);
                            }
                            if (strncmp(data, "4350", 4) == 0)
                            {
                                output = dataQueue.front();
                                MessageController::WebSocketServer::dataSender(req, output, WCT_TXTDATA);
                            }
                        })
                    .OnDisConnection(
                        [&](auto *req)
                        { return; })
                    .OnError(
                        [&](auto *req, auto *data, auto opcode, auto len)
                        {
                            std::string output = "4300/ERROR!";
                            MessageController::WebSocketServer::dataSender(req, output, WCT_TXTDATA);
                        })
                    .Run();
            }

            if (VC.EnableVideoControll)
            {
                DetectedVideo = CameraScanner(VideoList);
                std::ifstream config("/etc/APMCAMConfig.json");
                std::string content((std::istreambuf_iterator<char>(config)),
                                    (std::istreambuf_iterator<char>()));
                VideoJSONConfig = nlohmann::json::parse(content);
                VideoJSONConfig["VideoList"] = {};
                for (size_t i = 0; i < DetectedVideo; i++)
                {
                    VideoJSONConfig["VideoList"].push_back(VideoList[i]);
                }
                CAMBufferSize = VideoJSONConfig["QueueSize"].get<int>();

                std::ofstream APMCAMConfig;
                APMCAMConfig.open("/etc/APMCAMConfig.json");
                APMCAMConfig << std::setw(4) << VideoJSONConfig << std::endl;
                APMCAMConfig.close();
                for (size_t i = 0; i < VideoJSONConfig["UsingVideo"].size(); i++)
                {
                    try
                    {
                        CameraControll[i] = false;
                        std::string videoName;
                        for (auto &x : VideoJSONConfig["UsingVideo"][i].items())
                        {
                            videoName = x.key();
                        }
                        V4L2Tools::V4l2Info TmpInfo;
                        TmpInfo.ImgHeight = VideoJSONConfig["UsingVideo"][i][videoName]["Height"].get<int>();
                        TmpInfo.ImgWidth = VideoJSONConfig["UsingVideo"][i][videoName]["Width"].get<int>();
                        TmpInfo.Is_fastMode = VideoJSONConfig["UsingVideo"][i][videoName]["fastMode"].get<bool>();
                        TmpInfo.FrameBuffer = 1;
                        TmpInfo.PixFormat = V4L2_PIX_FMT_YUYV;
                        V4L2Main[i].reset(new V4L2Tools::V4L2Drive(videoName, TmpInfo));
                        int ThreadSigned = VideoJSONConfig["UsingVideo"][i][videoName]["threadSigned"].get<int>();
                        CameraThread[i] = std::thread(
                            [&]
                            {
                                int CAMNUM = i;
                                int CAMLoop = 0;
                                int CAMNext = 0;
                                int CAMStart = 0;
                                int CAMEnd = 0;

                                bool IsX11Enable = VideoJSONConfig["UsingVideo"][CAMNUM][videoName]["X11Direct"]["enable"].get<bool>();
                                int FPS = VideoJSONConfig["UsingVideo"][CAMNUM][videoName]["FPS"].get<int>();
                                int CAMWidth = VideoJSONConfig["UsingVideo"][CAMNUM][videoName]["Width"].get<int>();
                                int CAMHeight = VideoJSONConfig["UsingVideo"][CAMNUM][videoName]["Height"].get<int>();
                                if (VideoJSONConfig["UsingVideo"][CAMNUM][videoName]["RTMP"]["enable"].get<bool>())
                                {
                                    RTMPCameraSigned = CAMNUM;
                                    RTMPWidth = VideoJSONConfig["UsingVideo"][CAMNUM][videoName]["RTMP"]["Width"].get<int>();
                                    RTMPHeight = VideoJSONConfig["UsingVideo"][CAMNUM][videoName]["RTMP"]["Height"].get<int>();
                                    RTMPFPS = FPS;
                                }
                                std::string x11Name = "RuAPS_X11Direct" + std::to_string(CAMNUM);
                                if (IsX11Enable)
                                {
                                    cv::namedWindow(x11Name, cv::WND_PROP_FULLSCREEN);
                                    cv::setWindowProperty(x11Name, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
                                }

                                cv::Mat TmpMat;
                                unsigned char *TmpBGR = V4L2Main[CAMNUM]->RGB24DataInit();
                                CameraControll[CAMNUM] = true;
#ifdef DEBUG
                                std::cout << "[VideoInfo]Camera thread:" << videoName << " is reg finish\n";
#endif
                                while (CameraControll[CAMNUM])
                                {
                                    CAMStart = micros();
                                    CAMNext = CAMStart - CAMEnd;
                                    //
                                    RawV4L2Data[CAMNUM] = V4L2Main[CAMNUM]->V4L2Read();
                                    V4L2Main[CAMNUM]->yuyv2rgb24(RawV4L2Data[CAMNUM], TmpBGR, CAMWidth, CAMHeight);
                                    TmpMat = cv::Mat(CAMHeight, CAMWidth, CV_8UC3, TmpBGR);
                                    cv::cvtColor(TmpMat, TmpMat, cv::COLOR_RGB2BGR);
                                    if (IsX11Enable)
                                    {
                                        cv::imshow(x11Name, TmpMat);
                                        cv::waitKey(10);
                                    }
                                    CAMBuffer[CAMNUM].pushFrame(TmpMat);
                                    if (CAMBuffer[CAMNUM].size() > CAMBufferSize)
                                    {
                                        CAMBuffer[CAMNUM].clearBuffer();
                                    }
                                    //
                                    CAMEnd = micros();
                                    CAMLoop = CAMEnd - CAMStart;
                                    if (CAMLoop + CAMNext > ((1 / FPS) * 1000000.f) | CAMNext < 0)
                                    {
                                        usleep(50);
                                    }
                                    else
                                    {
                                        usleep(((1 / FPS) * 1000000.f) - CAMLoop - CAMNext);
                                    }
                                    CAMEnd = micros();
                                }
                            });
                        cpu_set_t cpuset;
                        CPU_ZERO(&cpuset);
                        CPU_SET(ThreadSigned, &cpuset);
                        int rc = pthread_setaffinity_np(CameraThread[i].native_handle(), sizeof(cpu_set_t), &cpuset);
                        while (!CameraControll[i])
                            usleep(50000);
                    }
                    catch (const int &e)
                    {
                    }
                }
            }

            if (VC.EnableVideoStream)
            {
                RTMPServer.reset(new RTMPPusher(
                    {
                        ._flag_Width = RTMPWidth,
                        ._flag_Height = RTMPHeight,
                        ._flag_Bitrate = 300000,
                        ._flag_FPS = 15,
                        ._flag_BufferSize = 12,
                        ._flag_RTMPStreamURL = "rtmp://0.0.0.0/live",
                        ._flag_CodeCProfile = "high444",
                        ._flag_StreamFormat = "flv",
                    }));
                RTMPServer->RTMPEncodecerInit();
                VideoStreamThread = std::thread(
                    [&]
                    {
                        StreamControll = true;
                        while (StreamControll)
                        {
                            if (CAMBuffer[RTMPCameraSigned].size() > 0)
                            {
                                cv::Mat TmpMat = CAMBuffer[RTMPCameraSigned].getFrame();
                                if (!TmpMat.empty())
                                {
                                    RTMPFrame = RTMPServer->CVMattToAvframe(&TmpMat, RTMPFrame);
                                    RTMPServer->RTMPSender(RTMPFrame);
                                }
                            }
                            usleep((1.f / RTMPFPS) * 1000000.f);
                        }
                    });
                cpu_set_t cpuset;
                CPU_ZERO(&cpuset);
                CPU_SET(0, &cpuset);
                int rc = pthread_setaffinity_np(VideoStreamThread.native_handle(), sizeof(cpu_set_t), &cpuset);
            }
        };

        Action &&UserAction()
        {
            if (CC.LUAAllowUserScript)
            {
                LuaUserLocal.LuaLocalInit();
                LuaUserLocal.LuaLocalLoad((char *)"../User/Lua/LuaTester.lua");
                //
                LuaUserLocal.LuaLocalFunctionPush(
                    "LF_GetAPSRTStatus", [&](lua_State *L) -> int
                    {
                        lua_newtable(L);
                        lua_pushstring(L, "Real_Pitch");
                        lua_pushinteger(L, SF._uORB_MPU_Data._uORB_Real_Pitch);
                        lua_settable(L, -3);

                        lua_pushstring(L, "Real__Roll");
                        lua_pushinteger(L, SF._uORB_MPU_Data._uORB_Real__Roll);
                        lua_settable(L, -3);

                        lua_pushstring(L, "Speed_X");
                        lua_pushinteger(L, SF._uORB_True_Speed_X);
                        lua_settable(L, -3);

                        lua_pushstring(L, "Speed_Y");
                        lua_pushinteger(L, SF._uORB_True_Speed_Y);
                        lua_settable(L, -3);

                        lua_pushstring(L, "Speed_Z");
                        lua_pushinteger(L, SF._uORB_True_Speed_Z);
                        lua_settable(L, -3);

                        lua_pushstring(L, "Move_X");
                        lua_pushinteger(L, SF._uORB_True_Movement_X);
                        lua_settable(L, -3);

                        lua_pushstring(L, "Move_Y");
                        lua_pushinteger(L, SF._uORB_True_Movement_Y);
                        lua_settable(L, -3);

                        lua_pushstring(L, "Move_Z");
                        lua_pushinteger(L, SF._uORB_True_Movement_Z);
                        lua_settable(L, -3);

                        lua_pushstring(L, "AltHoldTarget");
                        lua_pushinteger(L, PF._uORB_PID_AltHold_Target);
                        lua_settable(L, -3);

                        lua_pushstring(L, "FlowQuality");
                        lua_pushinteger(L, SF._uORB_Flow_Quality);
                        lua_settable(L, -3);

                        lua_pushstring(L, "APMode");
                        lua_pushinteger(L, AF.AutoPilotMode);
                        lua_settable(L, -3);
                        return 1;
                    });
                LuaUserLocal.LuaLocalFunctionPush(
                    "LF_GetAPSRCValue", [&](lua_State *L) -> int
                    {
                        lua_newtable(L);
                        for (int i = 0; i < 15; i++)
                        {
                            lua_pushinteger(L, i + 1);
                            lua_pushinteger(L, RF._uORB_RC_Channel_PWM[i]);
                            lua_settable(L, -3);
                        }
                        return 1;
                    });
                LuaUserLocal.LuaLocalFunctionPush(
                    "LF_GetLUARTStatus", [&](lua_State *L) -> int
                    {
                        lua_pushinteger(L, UserTimerLoop);
                        lua_pushinteger(L, UserTimerError);
                        return 2;
                    });
                LuaUserLocal.LuaLocalFunctionPush(
                    "LF_SetAPSServo", [&](lua_State *L) -> int
                    {
                        int pin = lua_tointeger(L, 1);
                        int _on = lua_tointeger(L, 2);
                        int off = lua_tointeger(L, 3);
                        APMControllerServo(pin, _on, off);
                        return 0;
                    });
                LuaUserLocal.LuaLocalFunctionPush(
                    "LF_SetUserPosition", [&](lua_State *L) -> int
                    {
                        int x = lua_tointeger(L, 1);
                        int y = lua_tointeger(L, 2);
                        int z = lua_tointeger(L, 3);
                        bool reset = lua_toboolean(L, 4);
                        APMControllerPosition(x, y, z, reset);
                        return 0;
                    });
                LuaUserLocal.LuaLocalFunctionPush(
                    "LF_SetUserSpeed", [&](lua_State *L) -> int
                    {
                        int x = lua_tointeger(L, 1);
                        int y = lua_tointeger(L, 2);
                        int z = lua_tointeger(L, 3);
                        APMControllerSpeed(x, y, z);
                        return 0;
                    });
                LuaUserLocal.LuaLocalFunctionPush(
                    "LF_RequestDISARM", [&](lua_State *L) -> int
                    {
                        bool Request = lua_toboolean(L, 1);
                        AF._flag_ESC_DISARMED_Request = Request;
                        return 0;
                    });
                LuaUserLocal.LuaLocalRun();
                LuaUserLocal.LuaLocalCall("Setup", 0, 0);
                UserTimerFreq = LuaUserLocal.LuaLocalVarGet("LUARTFreq");
                UserTimerMax = (float)1 / UserTimerFreq * 1000000.f;
                UserResetChannel = LuaUserLocal.LuaLocalVarGet("LUARECH") - 1;
                UserResetValue = LuaUserLocal.LuaLocalVarGet("LUAREVA");
                UserRunThread = std::thread(
                    [&]
                    {
                        bool IsReloaded = true;
                        bool IsReloadRequire = false;
                        while (true)
                        {
                            UserTimerStart = micros();
                            UserTimerNext = UserTimerStart - UserTimerEnd;
                            //
                            if (UserResetValue - 100 < RF._uORB_RC_Channel_PWM[UserResetChannel] && RF._uORB_RC_Channel_PWM[UserResetChannel] < UserResetValue + 100)
                            {
                                if (IsReloaded)
                                    IsReloadRequire = false;
                                else
                                    IsReloadRequire = true;
                            }
                            else
                            {
                                IsReloaded = false;
                                IsReloadRequire = false;
                            }
                            if (IsReloadRequire && !IsReloaded)
                            {
                                LuaUserLocal.LuaLocalLoad((char *)"../User/Lua/LuaTester.lua");
                                LuaUserLocal.LuaLocalRun();
                                LuaUserLocal.LuaLocalCall("Setup", 0, 0);
                                UserTimerFreq = LuaUserLocal.LuaLocalVarGet("LUARTFreq");
                                UserTimerMax = (float)1 / UserTimerFreq * 1000000.f;
                                UserResetChannel = LuaUserLocal.LuaLocalVarGet("LUARECH") - 1;
                                UserResetValue = LuaUserLocal.LuaLocalVarGet("LUAREVA");
                                UserTimerError = 0;
                                IsReloaded = true;
                            }
                            else
                                LuaUserLocal.LuaLocalCall("Loop", 0, 0);
                            //
                            UserTimerEnd = micros();
                            UserTimerLoop = UserTimerEnd - UserTimerStart;
                            if (UserTimerLoop + UserTimerNext > UserTimerMax | UserTimerNext < 0)
                            {
                                usleep(50);
                                UserTimerError++;
                            }
                            else
                            {
                                usleep(UserTimerMax - UserTimerLoop - UserTimerNext);
                            }
                            UserTimerEnd = micros();
                        }
                    });
            }

            if (CC.CPPAllowUserScript)
            {
                UserAPICPP::RequestFrame = [&](cv::Mat &img, int Queue) {};
                UserAPICPP::GetRTStatus = [&](float *Status)
                {
                    Status[0] = SF._uORB_MPU_Data._uORB_Real_Pitch;
                    Status[1] = SF._uORB_MPU_Data._uORB_Real__Roll;
                    Status[2] = SF._uORB_True_Speed_X;
                    Status[3] = SF._uORB_True_Speed_Y;
                    Status[4] = SF._uORB_True_Speed_Z;
                    Status[5] = SF._uORB_True_Movement_X;
                    Status[6] = SF._uORB_True_Movement_Y;
                    Status[7] = SF._uORB_True_Movement_Z;
                    Status[8] = PF._uORB_PID_AltHold_Target;
                    Status[9] = SF._uORB_Flow_Quality;
                    Status[10] = AF.AutoPilotMode;
                    Status[11] = UserTimerError;
                    Status[12] = UserTimerLoop;
                };
                UserAPICPP::GetRCValues = [&](float *Value)
                {
                    for (int i = 0; i < 10; i++)
                    {
                        Value[i] = RF._uORB_RC_Channel_PWM[i];
                    }
                };
                UserAPICPP::SetServo = [&](int pin, int on, int off)
                { APMControllerServo(pin, on, off); };
                UserAPICPP::RequestDISARM = [&](bool DISARM)
                { AF._flag_ESC_DISARMED_Request = DISARM; };
                UserAPICPP::SetUserSpeed = [&](int x, int y, int z)
                { APMControllerSpeed(x, y, z); };
                UserAPICPP::SetUserPosition = [&](int x, int y, int z, bool reset)
                { APMControllerPosition(x, y, z, reset); };
                UserAPICPP::FakeRCImplment = [&](int *RCChannel, bool IsError)
                {
                    APMControllerFakeRC(RCChannel, IsError);
                };
                UserAPICPP::UserSetup();
                UserRunThread = std::thread(
                    [&]
                    {
                        bool IsReloaded = true;
                        bool IsReloadRequire = false;
                        while (true)
                        {
                            UserTimerStart = micros();
                            UserTimerNext = UserTimerStart - UserTimerEnd;
                            //
                            UserAPICPP::UserLoop();
                            //
                            UserTimerEnd = micros();
                            UserTimerLoop = UserTimerEnd - UserTimerStart;
                            if (UserTimerLoop + UserTimerNext > UserTimerMax | UserTimerNext < 0)
                            {
                                usleep(50);
                                UserTimerError++;
                            }
                            else
                            {
                                usleep(UserTimerMax - UserTimerLoop - UserTimerNext);
                            }
                            UserTimerEnd = micros();
                        }
                    });
            }

            return std::move(*this);
        };

        Action &&Wait()
        {
            if (CC.EnableControllerDEBUG)
                TaskThreadBlock();
            ControllerThread.join();
            MessageServer.Wait();
            return std::move(*this);
        };

    private:
        int DetectedVideo = 0;
        std::string VideoList[5];
        std::string dataBuffer[256];
        nlohmann::json VideoJSONConfig;

        int CameraCount = 0;
        int CAMBufferSize = 2;
        bool CameraControll[5];
        unsigned char *RawV4L2Data[5];
        FrameBuffer<cv::Mat> CAMBuffer[5];

        std::thread ServerTread;
        std::thread ControllerThread;
        std::thread VideoStreamThread;
        std::thread CameraThread[5];

        std::queue<std::string> dataQueue;
        MessageController::WebSocketServer MessageServer;
        std::unique_ptr<V4L2Tools::V4L2Drive> V4L2Main[5];

        bool StreamControll;
        int RTMPFPS;
        int RTMPWidth;
        int RTMPHeight;
        int RTMPCameraSigned;
        AVFrame *RTMPFrame = nullptr;
        std::unique_ptr<RTMPPusher> RTMPServer;

        int UserTimerStart = 0;
        int UserTimerEnd = 0;
        int UserTimerNext = 0;
        int UserTimerLoop = 0;
        int UserTimerFreq = 100.f;
        int UserTimerMax = (float)1 / 100.f * 1000000.f;
        int UserTimerError = 0;
        int UserResetChannel = 10;
        int UserResetValue = 1933;
        std::thread UserRunThread;
        LuaLocal LuaUserLocal;

        void configAPMSettle(const char *configDir, APMSettinngs &APMInit)
        {
            std::ifstream config(configDir);
            std::string content((std::istreambuf_iterator<char>(config)),
                                (std::istreambuf_iterator<char>()));
            nlohmann::json Configdata = nlohmann::json::parse(content);
            //==========================================================Device Type=======/
            APMInit.DC.RC_Type = Configdata["Type_RC"].get<int>();
            APMInit.DC.MPU9250_Type = Configdata["Type_MPU9250"].get<int>();
            APMInit.DC.IMUFilter_Type = Configdata["Type_IMUFilter"].get<int>();

            APMInit.DC.__RCDevice = Configdata["__RCDevice"].get<std::string>(),
            APMInit.DC.__GPSDevice = Configdata["__GPSDevice"].get<std::string>(),
            APMInit.DC.__FlowDevice = Configdata["__FlowDevice"].get<std::string>(),

            APMInit.DC._IsGPSEnable = Configdata["_IsGPSEnable"].get<bool>();
            APMInit.DC._IsFlowEnable = Configdata["_IsFlowEnable"].get<bool>();
            APMInit.DC._IsMS5611Enable = Configdata["_IsMS5611Enable"].get<bool>();
            APMInit.DC._IsRCSafeEnable = Configdata["_IsRCSafeEnable"].get<bool>();

            APMInit.DC.IMU_Freqeuncy = Configdata["IMU_Freqeucy"].get<int>();
            APMInit.DC.RXT_Freqeuncy = Configdata["RXT_Freqeucy"].get<int>();
            APMInit.DC.ESC_Freqeuncy = Configdata["ESC_Freqeucy"].get<int>();
            //==========================================================Controller cofig==/
            APMInit.RC._flag_RC_Min_PWM_Value = Configdata["_flag_RC_Min_PWM_Value"].get<int>();
            APMInit.RC._flag_RC_Mid_PWM_Value = Configdata["_flag_RC_Mid_PWM_Value"].get<int>();
            APMInit.RC._flag_RC_Max_PWM_Value = Configdata["_flag_RC_Max_PWM_Value"].get<int>();
            //===========================
            APMInit.RC._flag_RC_ARM_PWM_Value = Configdata["_flag_RC_ARM_PWM_Value"].get<int>();
            APMInit.RC._flag_RC_ARM_PWM_Channel = Configdata["_flag_RC_ARM_PWM_Channel"].get<int>();

            APMInit.RC._flag_RC_AP_ManualHold_PWM_Value = Configdata["_flag_RC_AP_ManualHold_PWM_Value"].get<int>();
            APMInit.RC._flag_RC_AP_ManualHold_PWM_Channel = Configdata["_flag_RC_AP_ManualHold_PWM_Channel"].get<int>();

            APMInit.RC._flag_RC_AP_AutoStable_PWM_Value = Configdata["_flag_RC_AP_AutoStable_PWM_Value"].get<int>();
            APMInit.RC._flag_RC_AP_AutoStable_PWM_Channel = Configdata["_flag_RC_AP_AutoStable_PWM_Channel"].get<int>();

            APMInit.RC._flag_RC_AP_AltHold_PWM_Value = Configdata["_flag_RC_AP_AltHold_PWM_Value"].get<int>();
            APMInit.RC._flag_RC_AP_AltHold_PWM_Channel = Configdata["_flag_RC_AP_AltHold_PWM_Channel"].get<int>();

            APMInit.RC._flag_RC_AP_SpeedHold_PWM_Value = Configdata["_flag_RC_AP_SpeedHold_PWM_Value"].get<int>();
            APMInit.RC._flag_RC_AP_SpeedHold_PWM_Channel = Configdata["_flag_RC_AP_SpeedHold_PWM_Channel"].get<int>();

            APMInit.RC._flag_RC_AP_PositionHold_PWM_Value = Configdata["_flag_RC_AP_PositionHold_PWM_Value"].get<int>();
            APMInit.RC._flag_RC_AP_PositionHold_PWM_Channel = Configdata["_flag_RC_AP_PositionHold_PWM_Channel"].get<int>();

            APMInit.RC._flag_RC_AP_UserAuto_PWM_Value = Configdata["_flag_RC_AP_UserAuto_PWM_Value"].get<int>();
            APMInit.RC._flag_RC_AP_UserAuto_PWM_Channel = Configdata["_flag_RC_AP_UserAuto_PWM_Channel"].get<int>();
            //===========================
            APMInit.RC._flag_RCIsReserv__Roll = Configdata["_flag_RCIsReserv__Roll"].get<int>();
            APMInit.RC._flag_RCIsReserv_Pitch = Configdata["_flag_RCIsReserv_Pitch"].get<int>();
            APMInit.RC._flag_RCIsReserv___Yaw = Configdata["_flag_RCIsReserv___Yaw"].get<int>();
            //==========================================================ESC cofig=========/
            APMInit.OC._flag_A1_Pin = Configdata["_flag_A1_Pin"].get<int>();
            APMInit.OC._flag_A2_Pin = Configdata["_flag_A2_Pin"].get<int>();
            APMInit.OC._flag_B1_Pin = Configdata["_flag_B1_Pin"].get<int>();
            APMInit.OC._flag_B2_Pin = Configdata["_flag_B2_Pin"].get<int>();
            APMInit.OC._flag_YAWOut_Reverse = Configdata["_flag_YAWOut_Reverse"].get<float>();
            //==================================================================PID cofig==/
            APMInit.PC._flag_PID_P__Roll_Gain = Configdata["_flag_PID_P__Roll_Gain"].get<float>();
            APMInit.PC._flag_PID_P_Pitch_Gain = Configdata["_flag_PID_P_Pitch_Gain"].get<float>();
            APMInit.PC._flag_PID_P___Yaw_Gain = Configdata["_flag_PID_P___Yaw_Gain"].get<float>();
            APMInit.PC._flag_PID_P_Alt_Gain = Configdata["_flag_PID_P_Alt_Gain"].get<float>();
            APMInit.PC._flag_PID_P_PosX_Gain = Configdata["_flag_PID_P_PosX_Gain"].get<float>();
            APMInit.PC._flag_PID_P_PosY_Gain = Configdata["_flag_PID_P_PosY_Gain"].get<float>();
            APMInit.PC._flag_PID_P_SpeedZ_Gain = Configdata["_flag_PID_P_SpeedZ_Gain"].get<float>();
            APMInit.PC._flag_PID_P_SpeedX_Gain = Configdata["_flag_PID_P_SpeedX_Gain"].get<float>();
            APMInit.PC._flag_PID_P_SpeedY_Gain = Configdata["_flag_PID_P_SpeedY_Gain"].get<float>();

            APMInit.PC._flag_PID_I__Roll_Gain = Configdata["_flag_PID_I__Roll_Gain"].get<float>();
            APMInit.PC._flag_PID_I_Pitch_Gain = Configdata["_flag_PID_I_Pitch_Gain"].get<float>();
            APMInit.PC._flag_PID_I___Yaw_Gain = Configdata["_flag_PID_I___Yaw_Gain"].get<float>();
            APMInit.PC._flag_PID_I_Alt_Gain = Configdata["_flag_PID_I_Alt_Gain"].get<float>();
            APMInit.PC._flag_PID_I_PosX_Gain = Configdata["_flag_PID_I_PosX_Gain"].get<float>();
            APMInit.PC._flag_PID_I_PosY_Gain = Configdata["_flag_PID_I_PosY_Gain"].get<float>();
            APMInit.PC._flag_PID_I_SpeedZ_Gain = Configdata["_flag_PID_I_SpeedZ_Gain"].get<float>();
            APMInit.PC._flag_PID_I_SpeedX_Gain = Configdata["_flag_PID_I_SpeedX_Gain"].get<float>();
            APMInit.PC._flag_PID_I_SpeedY_Gain = Configdata["_flag_PID_I_SpeedY_Gain"].get<float>();
            APMInit.PC._flag_PID_I__Roll_Max__Value = Configdata["_flag_PID_I__Roll_Max__Value"].get<float>();
            APMInit.PC._flag_PID_I_Pitch_Max__Value = Configdata["_flag_PID_I_Pitch_Max__Value"].get<float>();
            APMInit.PC._flag_PID_I___Yaw_Max__Value = Configdata["_flag_PID_I___Yaw_Max__Value"].get<float>();

            APMInit.PC._flag_PID_D__Roll_Gain = Configdata["_flag_PID_D__Roll_Gain"].get<float>();
            APMInit.PC._flag_PID_D_Pitch_Gain = Configdata["_flag_PID_D_Pitch_Gain"].get<float>();
            APMInit.PC._flag_PID_D___Yaw_Gain = Configdata["_flag_PID_D___Yaw_Gain"].get<float>();
            APMInit.PC._flag_PID_D_SpeedZ_Gain = Configdata["_flag_PID_D_SpeedZ_Gain"].get<float>();
            APMInit.PC._flag_PID_D_SpeedX_Gain = Configdata["_flag_PID_D_SpeedX_Gain"].get<float>();
            APMInit.PC._flag_PID_D_SpeedY_Gain = Configdata["_flag_PID_D_SpeedY_Gain"].get<float>();

            APMInit.PC._flag_PID_Hover_Throttle = Configdata["_flag_PID_Hover_Throttle"].get<float>();
            APMInit.PC._flag_PID_Level_Max = Configdata["_flag_PID_Level_Max"].get<float>();
            APMInit.PC._flag_PID_Alt_Level_Max = Configdata["_flag_PID_Alt_Level_Max"].get<float>();
            APMInit.PC._flag_PID_Pos_Level_Max = Configdata["_flag_PID_Pos_Level_Max"].get<float>();

            APMInit.PC._flag_PID_Takeoff_Altitude = Configdata["_flag_PID_Takeoff_Altitude"].get<float>();
            APMInit.PC._flag_PID_Alt_Speed_Max = Configdata["_flag_PID_Alt_Speed_Max"].get<float>();
            APMInit.PC._flag_PID_Alt_Accel_Max = Configdata["_flag_PID_Alt_Accel_Max"].get<float>();
            APMInit.PC._flag_PID_PosMan_Speed_Max = Configdata["_flag_PID_PosMan_Speed_Max"].get<float>();
            APMInit.PC._flag_PID_Pos_Accel_Max = Configdata["_flag_PID_Pos_Accel_Max"].get<float>();
            APMInit.PC._flag_PID_Pos_Speed_Max = Configdata["_flag_PID_Pos_Speed_Max"].get<float>();
            APMInit.PC._flag_PID_AngleRate_Gain = Configdata["_flag_PID_AngleRate_Gain"].get<float>();
            //==============================================================Sensors cofig==/
            APMInit.SC._flag_MPU9250_A_X_Cali = Configdata["_flag_MPU9250_A_X_Cali"].get<double>();
            APMInit.SC._flag_MPU9250_A_Y_Cali = Configdata["_flag_MPU9250_A_Y_Cali"].get<double>();
            APMInit.SC._flag_MPU9250_A_Z_Cali = Configdata["_flag_MPU9250_A_Z_Cali"].get<double>();
            APMInit.SC._flag_MPU9250_A_X_Scal = Configdata["_flag_MPU9250_A_X_Scal"].get<double>();
            APMInit.SC._flag_MPU9250_A_Y_Scal = Configdata["_flag_MPU9250_A_Y_Scal"].get<double>();
            APMInit.SC._flag_MPU9250_A_Z_Scal = Configdata["_flag_MPU9250_A_Z_Scal"].get<double>();

            APMInit.SC._flag_COMPASS_Y_Scaler = Configdata["_flag_COMPASS_Y_Scaler"].get<double>();
            APMInit.SC._flag_COMPASS_Z_Scaler = Configdata["_flag_COMPASS_Z_Scaler"].get<double>();
            APMInit.SC._flag_COMPASS_X_Offset = Configdata["_flag_COMPASS_X_Offset"].get<double>();
            APMInit.SC._flag_COMPASS_Y_Offset = Configdata["_flag_COMPASS_Y_Offset"].get<double>();
            APMInit.SC._flag_COMPASS_Z_Offset = Configdata["_flag_COMPASS_Z_Offset"].get<double>();
            //==============================================================Filter config==/
            APMInit.FC._flag_Filter_Gryo_Type = Configdata["_flag_Filter_Gryo_Type"].get<double>();
            APMInit.FC._flag_Filter_GYaw_CutOff = Configdata["_flag_Filter_GYaw_CutOff"].get<double>();
            APMInit.FC._flag_Filter_Gryo_CutOff = Configdata["_flag_Filter_Gryo_CutOff"].get<double>();
            APMInit.FC._flag_Filter_Gryo_NotchFreq = Configdata["_flag_Filter_Gryo_NotchFreq"].get<double>();
            APMInit.FC._flag_Filter_Gryo_NotchCutOff = Configdata["_flag_Filter_Gryo_NotchCutOff"].get<double>();
            APMInit.FC._flag_Filter_Gryo_DynamicNotchRange = Configdata["_flag_Filter_Gryo_DynamicNotchRange"].get<double>();
            APMInit.FC._flag_Filter_Gryo_DynamicNotchEnable = Configdata["_flag_Filter_Gryo_DynamicNotchEnable"].get<bool>();
            APMInit.FC._flag_Filter_Gryo_DynamicNotchMinFreq = Configdata["_flag_Filter_Gryo_DynamicNotchMinFreq"].get<double>();
            APMInit.FC._flag_Filter_Accel_Type = Configdata["_flag_Filter_Accel_Type"].get<double>();
            APMInit.FC._flag_Filter_Accel_CutOff = Configdata["_flag_Filter_Accel_CutOff"].get<double>();
            APMInit.FC._flag_Filter_AngleMix_Alpha = Configdata["_flag_Filter_AngleMix_Alpha"].get<double>();

            APMInit.FC._flag_Baro_Trust_Beta = Configdata["_flag_Baro_Trust_Beta"].get<double>();
            APMInit.FC._flag_Accel_Trust_Beta = Configdata["_flag_Accel_Trust_Beta"].get<double>();
            APMInit.FC._flag_Sonar_Trust_Beta = Configdata["_flag_Sonar_Trust_Beta"].get<double>();
            APMInit.FC._flag_GPSAlt_Trust_Beta = Configdata["_flag_GPSAlt_Trust_Beta"].get<double>();
            APMInit.FC._flag_AccelBias_Trust_Beta = Configdata["_flag_AccelBias_Trust_Beta"].get<double>();

            APMInit.FC._flag_Filter_RC_CutOff = Configdata["_flag_Filter_RC_CutOff"].get<double>();
            APMInit.FC._flag_Filter_AngleRate_CutOff = Configdata["_flag_Filter_AngleRate_CutOff"].get<double>();

            APMInit.FC._flag_Filter_PID_I_CutOff = Configdata["_flag_Filter_PID_I_CutOff"].get<double>();
            APMInit.FC._flag_Filter_PID_D_ST1_CutOff = Configdata["_flag_Filter_PID_D_ST1_CutOff"].get<double>();
            APMInit.FC._flag_Filter_PID_D_ST2_CutOff = Configdata["_flag_Filter_PID_D_ST2_CutOff"].get<double>();

            APMInit.FC._flag_GPS_Config_Beta = Configdata["_flag_GPS_Config_Beta"].get<double>();
            APMInit.FC._flag_Flow_Config_Beta = Configdata["_flag_Flow_Config_Beta"].get<double>();
            APMInit.FC._flag_Braking_Speed_Gain = Configdata["_flag_Braking_Speed_Gain"].get<double>();
            APMInit.FC._flag_Braking_AccelMax_Gain = Configdata["_flag_Braking_AccelMax_Gain"].get<double>();
        };

        template <typename T>
        T configRead(const char *configDir, const char *Target)
        {
            std::ifstream config(configDir);
            std::string content((std::istreambuf_iterator<char>(config)),
                                (std::istreambuf_iterator<char>()));
            nlohmann::json Configdata = nlohmann::json::parse(content);
            return Configdata[Target].get<T>();
        }

        template <typename T>
        void configWrite(const char *configDir, const char *Target, T obj)
        {
            std::ifstream config(configDir);
            std::string content((std::istreambuf_iterator<char>(config)),
                                (std::istreambuf_iterator<char>()));
            nlohmann::json Configdata = nlohmann::json::parse(content);
            Configdata[Target] = obj;
            std::ofstream configs;
            configs.open(configDir);
            configs << std::setw(4) << Configdata << std::endl;
            configs.close();
        };

        int CameraScanner(std::string *VideoList)
        {
            std::string device = "/dev";
            std::string video = "video";
            char *KEY_PTR = (char *)video.c_str();
            char *FILE_PTR = (char *)device.c_str();

            DIR *dir;
            struct dirent *ptr;
            char base[1000];

            if ((dir = opendir(FILE_PTR)) == NULL)
            {
            }

            int i = 0;
            while ((ptr = readdir(dir)) != NULL)
            {
                std::string name = "";
                name = (char *)ptr->d_name;
                if (name.find("video") != std::string::npos)
                {
                    std::string allName = device + "/" + name;
                    VideoList[i] = allName;
                    i++;
                }
            }
            closedir(dir);
            return i;
        }
    };
}; // namespace Action
