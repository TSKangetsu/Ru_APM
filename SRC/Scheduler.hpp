#include <queue>
#include <thread>
#include <fstream>
#include "_Excutable/Drive_Json.hpp"
#include "RPiSingleAPM/src/SingleAPM.hpp"
#include "MessageController/MessageController.hpp"
using json = nlohmann::json;
using namespace SingleAPMAPI;

#define Server_WebSocket 0

namespace Action
{
    class ConfigCenter
    {
    protected:
        struct VideoConfig
        {
        } VC;
        struct MessageConfig
        {
            int ServerType;
        } MC;
        struct ControllerConfig
        {
        } CC;
    };

    class Action : private ConfigCenter, private RPiSingleAPM
    {
    public:
        Action()
        {
            {
                configSettle("/etc/APMconfig.json", setting);
                RPiSingleAPMInit(setting);
                IMUSensorsTaskReg();
                ControllerTaskReg();
                AltholdSensorsTaskReg();
                PositionTaskReg();
                ESCUpdateTaskReg();
                ControllerThread = std::thread([&] {
                    while (true)
                    {
                        if (dataQueue.size() > 2)
                            dataQueue.pop();
                        json OutputJSON = {
                            {"type", 4200},
                            {"flyMode", MessageController::StringRounder(AF.AutoPilotMode, 2)},
                            {"ESCA1", MessageController::StringRounder(EF._uORB_A1_Speed, 2)},
                            {"ESCA2", MessageController::StringRounder(EF._uORB_A2_Speed, 2)},
                            {"ESCB1", MessageController::StringRounder(EF._uORB_B1_Speed, 2)},
                            {"ESCB2", MessageController::StringRounder(EF._uORB_B2_Speed, 2)},
                            {"GRYOPitch", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Gryo_Pitch, 2)},
                            {"GRYORoll", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Gryo__Roll, 2)},
                            {"GRYOYaw", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Gryo___Yaw, 2)},
                            {"AccelPitch", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Accel_Pitch, 2)},
                            {"AccelRoll", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Accel__Roll, 2)},
                            {"RealPitch", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Real_Pitch, 2)},
                            {"RealRoll", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Real__Roll, 2)},
                            {"FastPressure", MessageController::StringRounder(SF._Tmp_MS5611_PressureFast, 2)},
                            {"FinalPressure", MessageController::StringRounder(SF._uORB_MS5611_PressureFinal, 2)},
                            {"HoldPressure", MessageController::StringRounder(PF._uORB_PID_AltHold_Target, 2)},
                            {"HoldThrottle", MessageController::StringRounder(PF._uORB_PID_Alt_Throttle, 2)},
                            {"GPSLat", MessageController::StringRounder(SF._uORB_GPS_Lat_Smooth, 2)},
                            {"GPSLng", MessageController::StringRounder(SF._uORB_GPS_Lng_Smooth, 2)},
                            {"GPSSatCount", MessageController::StringRounder(SF._uORB_GPS_Data.satillitesCount, 2)},
                            {"RCRoll", MessageController::StringRounder(RF._uORB_RC_Out__Roll, 2)},
                            {"RCPitch", MessageController::StringRounder(RF._uORB_RC_Out_Pitch, 2)},
                            {"RCThrottle", MessageController::StringRounder(RF._uORB_RC_Out_Throttle, 2)},
                            {"RCYaw", MessageController::StringRounder(RF._uORB_RC_Out___Yaw, 2)},
                            {"RCYaw", MessageController::StringRounder(RF._uORB_RC_Out___Yaw, 2)},
                            {"RawADFX", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_MPU9250_ADF_X, 2)},
                            {"RawADFY", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_MPU9250_ADF_Y, 2)},
                            {"RawADFZ", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_MPU9250_ADF_Z, 2)},
                            {"RawFAX", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_MPU9250_AStaticFakeFD_X, 2)},
                            {"RawFAY", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_MPU9250_AStaticFakeFD_Y, 2)},
                            {"RawFAZ", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_MPU9250_AStaticFakeFD_Z, 2)},
                            {"RawSAX", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_MPU9250_A_Static_X, 2)},
                            {"RawSAY", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_MPU9250_A_Static_Y, 2)},
                            {"RawSAZ", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_MPU9250_A_Static_Z, 2)},
                            {"SpeedX", MessageController::StringRounder(SF._uORB_MPU_Speed_X, 2)},
                            {"SpeedY", MessageController::StringRounder(SF._uORB_MPU_Speed_Y, 2)},
                            {"SpeedZ", MessageController::StringRounder(SF._uORB_MPU_Speed_Z, 2)},
                            {"AccelX", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Acceleration_X, 2)},
                            {"AccelY", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Acceleration_Y, 2)},
                            {"AccelZ", MessageController::StringRounder(SF._uORB_MPU_Data._uORB_Acceleration_Z, 2)},
                            {"ClimbeRate", MessageController::StringRounder(SF._uORB_MS5611_ClimbeRate, 2)},
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
                        usleep(50000);
                    }
                });
            }
            sleep(1);
            if (MC.ServerType == Server_WebSocket)
            {
                MessageServer.WebSocketServerInit().OnConnection([&](auto *req, auto *data) {
                                                       std::string output = "4000";
                                                       MessageController::WebSocketServer::dataSender(req, output, WCT_TXTDATA);
                                                   })
                    .OnMessage([&](auto *req, auto *data, auto opcode, auto len) {
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
                    .OnDisConnection([&](auto *req) {
                        return;
                    })
                    .OnError([&](auto *req, auto *data, auto opcode, auto len) {
                        std::string output = "4300/ERROR!";
                        MessageController::WebSocketServer::dataSender(req, output, WCT_TXTDATA);
                    })
                    .Run();
            }
            MessageServer.Wait();
        };

    private:
        APMSettinngs setting;
        std::thread ServerTread;
        std::thread ControllerThread;
        std::string dataBuffer[256];
        std::queue<std::string> dataQueue;
        MessageController::WebSocketServer MessageServer;

        void configSettle(const char *configDir, APMSettinngs &APMInit)
        {
            std::ifstream config(configDir);
            std::string content((std::istreambuf_iterator<char>(config)),
                                (std::istreambuf_iterator<char>()));
            nlohmann::json Configdata = nlohmann::json::parse(content);
            //==========================================================Device Type=======/
            APMInit.RC_Type = Configdata["Type_RC"].get<int>();
            APMInit.MPU9250_Type = Configdata["Type_MPU9250"].get<int>();
            APMInit.IMUFilter_Type = Configdata["Type_IMUFilter"].get<int>();
            APMInit.IMUMixFilter_Type = Configdata["Type_IMUMixFilter"].get<int>();

            APMInit.__RCDevice = Configdata["__RCDevice"].get<std::string>(),
            APMInit.__GPSDevice = Configdata["__GPSDevice"].get<std::string>(),
            APMInit.__FlowDevice = Configdata["__FlowDevice"].get<std::string>(),

            APMInit._IsGPSEnable = Configdata["_IsGPSEnable"].get<int>();
            APMInit._IsFlowEnable = Configdata["_IsFlowEnable"].get<int>();
            APMInit._IsMS5611Enable = Configdata["_IsMS5611Enable"].get<int>();
            APMInit._IsRCSafeEnable = Configdata["_IsRCSafeEnable"].get<int>();
            //==========================================================Controller cofig==/
            APMInit._flag_RC_ARM_PWM_Value = Configdata["_flag_RC_ARM_PWM_Value"].get<int>();
            APMInit._flag_RC_Min_PWM_Value = Configdata["_flag_RC_Min_PWM_Value"].get<int>();
            APMInit._flag_RC_Mid_PWM_Value = Configdata["_flag_RC_Mid_PWM_Value"].get<int>();
            APMInit._flag_RC_Max_PWM_Value = Configdata["_flag_RC_Max_PWM_Value"].get<int>();

            APMInit._flag_RCIsReserv__Roll = Configdata["_flag_RCIsReserv__Roll"].get<int>();
            APMInit._flag_RCIsReserv_Pitch = Configdata["_flag_RCIsReserv_Pitch"].get<int>();
            APMInit._flag_RCIsReserv___Yaw = Configdata["_flag_RCIsReserv___Yaw"].get<int>();
            //==========================================================ESC cofig=========/
            APMInit._flag_A1_Pin = Configdata["_flag_A1_Pin"].get<int>();
            APMInit._flag_A2_Pin = Configdata["_flag_A2_Pin"].get<int>();
            APMInit._flag_B1_Pin = Configdata["_flag_B1_Pin"].get<int>();
            APMInit._flag_B2_Pin = Configdata["_flag_B2_Pin"].get<int>();
            //==================================================================PID cofig==/
            APMInit._flag_PID_P__Roll_Gain = Configdata["_flag_PID_P__Roll_Gain"].get<float>();
            APMInit._flag_PID_P_Pitch_Gain = Configdata["_flag_PID_P_Pitch_Gain"].get<float>();
            APMInit._flag_PID_P___Yaw_Gain = Configdata["_flag_PID_P___Yaw_Gain"].get<float>();
            APMInit._flag_PID_P_SpeedZ_Gain = Configdata["_flag_PID_P_SpeedZ_Gain"].get<float>();
            APMInit._flag_PID_P_GPS_Gain = Configdata["_flag_PID_P_GPS_Gain"].get<float>();

            APMInit._flag_PID_I__Roll_Gain = Configdata["_flag_PID_I__Roll_Gain"].get<float>();
            APMInit._flag_PID_I_Pitch_Gain = Configdata["_flag_PID_I_Pitch_Gain"].get<float>();
            APMInit._flag_PID_I___Yaw_Gain = Configdata["_flag_PID_I___Yaw_Gain"].get<float>();
            APMInit._flag_PID_I_SpeedZ_Gain = Configdata["_flag_PID_I_SpeedZ_Gain"].get<float>();
            APMInit._flag_PID_I__Roll_Max__Value = Configdata["_flag_PID_I__Roll_Max__Value"].get<float>();
            APMInit._flag_PID_I_Pitch_Max__Value = Configdata["_flag_PID_I_Pitch_Max__Value"].get<float>();
            APMInit._flag_PID_I___Yaw_Max__Value = Configdata["_flag_PID_I___Yaw_Max__Value"].get<float>();

            APMInit._flag_PID_D__Roll_Gain = Configdata["_flag_PID_D__Roll_Gain"].get<float>();
            APMInit._flag_PID_D_Pitch_Gain = Configdata["_flag_PID_D_Pitch_Gain"].get<float>();
            APMInit._flag_PID_D___Yaw_Gain = Configdata["_flag_PID_D___Yaw_Gain"].get<float>();
            APMInit._flag_PID_D_SpeedZ_Gain = Configdata["_flag_PID_D_SpeedZ_Gain"].get<float>();
            APMInit._flag_PID_D_GPS_Gain = Configdata["_flag_PID_D_GPS_Gain"].get<float>();

            APMInit._flag_PID_Hover_Throttle = Configdata["_flag_PID_Hover_Throttle"].get<float>();
            APMInit._flag_PID_Level_Max = Configdata["_flag_PID_Level_Max"].get<float>();
            APMInit._flag_PID_Alt_Level_Max = Configdata["_flag_PID_Alt_Level_Max"].get<float>();
            APMInit._flag_PID_GPS_Level_Max = Configdata["_flag_PID_GPS_Level_Max"].get<float>();
            //==============================================================Sensors cofig==/
            APMInit._flag_MPU9250_A_X_Cali = Configdata["_flag_MPU9250_A_X_Cali"].get<double>();
            APMInit._flag_MPU9250_A_Y_Cali = Configdata["_flag_MPU9250_A_Y_Cali"].get<double>();
            APMInit._flag_MPU9250_A_Z_Cali = Configdata["_flag_MPU9250_A_Z_Cali"].get<double>();
            APMInit._flag_MPU9250_A_X_Scal = Configdata["_flag_MPU9250_A_X_Scal"].get<double>();
            APMInit._flag_MPU9250_A_Y_Scal = Configdata["_flag_MPU9250_A_Y_Scal"].get<double>();
            APMInit._flag_MPU9250_A_Z_Scal = Configdata["_flag_MPU9250_A_Z_Scal"].get<double>();

            APMInit._flag_QMC5883L_Head_Asix = Configdata["_flag_QMC5883L_Head_Asix"].get<double>();
            APMInit._flag_QMC5883L_M_X_Offset = Configdata["_flag_QMC5883L_M_X_Offset"].get<double>();
            APMInit._flag_QMC5883L_M_Y_Offset = Configdata["_flag_QMC5883L_M_Y_Offset"].get<double>();
            APMInit._flag_QMC5883L_M_Z_Offset = Configdata["_flag_QMC5883L_M_Z_Offset"].get<double>();
            APMInit._flag_QMC5883L_M_Y_Scaler = Configdata["_flag_QMC5883L_M_Y_Scaler"].get<double>();
            APMInit._flag_QMC5883L_M_Z_Scaler = Configdata["_flag_QMC5883L_M_Z_Scaler"].get<double>();
            //===============================================================Update cofig==/
            APMInit.IMU_Freqeuncy = Configdata["IMU_Freqeucy"].get<int>();
            APMInit.RXT_Freqeuncy = Configdata["RXT_Freqeucy"].get<int>();
            APMInit.ESC_Freqeuncy = Configdata["ESC_Freqeucy"].get<int>();
        };

        void configWrite(const char *configDir, const char *Target, double obj)
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
    };
}; // namespace Action
