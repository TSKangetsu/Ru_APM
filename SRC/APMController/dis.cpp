//#include "M_APMController.hpp"
//APMMain::APMControl::APMControl()
//{
//	SingleAPMAPI::APMSettinngs APMSettle;
//	ACCSSDataSender = new APMNetWorkControl("192.168.137.1", 10086);
//	RPiSingleAPMInit(APMSettle);
//}
//
//int APMMain::APMControl::DataSender()
//{
//	sendOutDataBuff[0] = std::to_string(SF._uORB_Gryo_Pitch);
//	sendOutDataBuff[1] = std::to_string(SF._uORB_Gryo__Roll);
//	sendOutDataBuff[2] = std::to_string(SF._uORB_Gryo___Yaw);
//	sendOutDataBuff[3] = std::to_string(SF._uORB_Accel_Pitch);
//	sendOutDataBuff[4] = std::to_string(SF._uORB_Accel__Roll);
//	ACCSSDataSender->APMRealTimeDataSend(sendOutDataBuff);
//}
//
//int APMMain::APMControl::APMRuingSettle()
//{
//	std::thread AutoLevelingMain([&] {
//		while (true)
//		{
//			IMUSensorsParse();
//			ControlParse();
//			AttitudeUpdate();
//			SaftyChecking();
//			ESCUpdate();
//			ClockingTimer();
//		}
//		});
//
//	std::thread dataThread([&] {
//		while (true)
//		{
//			DataSender();
//		}
//		});
//
//	cpu_set_t cpuset;
//	CPU_ZERO(&cpuset);
//	CPU_SET(3, &cpuset);
//	int rc = pthread_setaffinity_np(AutoLevelingMain.native_handle(), sizeof(cpu_set_t), &cpuset);
//	AutoLevelingMain.join();
//}
//
////=---------------------------------=/
//APMMain::APMNetWorkControl::APMNetWorkControl(char* remoteHostIP, int remoteHostPort)
//{
//	remoteAddr = ipremote(remoteHostIP, remoteHostPort, 0, -1);
//	infoSocks = tcpconnect(remoteAddr, -1);
//
//	tcpsend(infoSocks, startChecksum, sizeof(startChecksum), -1);
//	tcpflush(infoSocks, -1);
//
//	tcprecv(infoSocks, dataBuff, 4, -1);
//	if (strcmp(dataBuff, comfirmChecksum) == 0)
//	{
//		tcpsend(infoSocks, IDCcomfirmsum, sizeof(IDCcomfirmsum), -1);
//		tcpflush(infoSocks, -1);
//
//		tcprecv(infoSocks, dataBuff, 7, -1);
//		strncpy(tmpDataBuff, dataBuff, 4);
//		if (strcmp(tmpDataBuff, IDGetSum) == 0)
//		{
//			strncpy(tmpDataBuff, dataBuff + 4, 4);
//			deviceID = std::stoi(tmpDataBuff);
//		}
//	}
//}
//
//APMMain::APMNetWorkControl::~APMNetWorkControl()
//{
//	tcpclose(infoSocks);
//}
//
//int APMMain::APMNetWorkControl::APMRealTimeDataSend(std::string* Data)
//{
//	tmpDataString = DataCheckSum;
//	tmpDataString += "/";
//	tmpDataString += std::to_string(deviceID);
//	tmpDataString += "/";
//	for (size_t i = 0; i < 5; i++)
//	{
//		tmpDataString += Data[i];
//		tmpDataString += "/";
//	}
//	std::cout << tmpDataString << "\n";
//	dataBuff = (char*)tmpDataString.c_str();
//	tcpsend(infoSocks, dataBuff, 256, -1);
//}
