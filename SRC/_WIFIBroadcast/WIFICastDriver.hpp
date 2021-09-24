#pragma once
#include "../_Excutable/Drive_Socket.hpp"
#define VideoTrans 0x68
#define DataETrans 0x69
#define SocketMTU 1490 - 5 //FCS auto add by driver or kerenl
#define HeaderSize 61

namespace WIFIBroadCast
{
    enum BroadCastType
    {
        VideoStream,
        DataStream,
    };

    class WIFICastDriver
    {
    public:
        WIFICastDriver(std::vector<std::string> Interfaces);
        ~WIFICastDriver();

        void WIFICastInject(uint8_t *data, int size, int InterfaceID, BroadCastType type, int delayUS);

        void WIFICastInjectMulti(uint8_t *data, int size, int delayUS){};
        void WIFICastInjectMultiBL(uint8_t *data, int size, int delayUS){};

    private:
        struct InjectPacketLLCInfo
        {
            const uint8_t RadioHeader[16] = {
                0x00, 0x00, 0x20, 0x00, 0xae, 0x40, 0x00, 0xa0,
                0x20, 0x08, 0x00, 0xa0, 0x20, 0x08, 0x00, 0x00};
            // Auto Complete by driver or kerenl
            const uint8_t RadioInfo[16] = {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            // Frame Control field and Duration
            const uint8_t Data80211Info[4] = {
                0x08, 0x02, 0x00, 0x00};
            // destination or BSS id should be zero
            const uint8_t DeviceInfo_DST[6] = {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            const uint8_t DeviceInfo_BSS[6] = {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            // Only SRC use for dentification when broadcast injection
            uint8_t DeviceInfo_SRC[6] = {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            // Only One of Then can be use In a Frame
            const uint8_t SquenceAndLLCVideo[7] = {
                0x00, 0x00, 0x00, 0x00, VideoTrans, 0xff, 0xff};
            const uint8_t SquenceAndLLCDataE[7] = {
                0x00, 0x00, 0x00, 0x00, DataETrans, 0xff, 0xff};
        } InjectPacketLLC;

        int InterfaceCount = 0;
        uint8_t FrameCounter = 0;
        uint8_t **PacketVideo;
        uint8_t **PacketDatae;
        std::vector<std::unique_ptr<Socket>> SocketInjectors;
    };

    class WIFIRecvDriver
    {
    };
}

//===============================================//
WIFIBroadCast::WIFICastDriver::WIFICastDriver(std::vector<std::string> Interfaces)
{
    InterfaceCount = Interfaces.size();
    PacketVideo = new uint8_t *[Interfaces.size()];
    PacketDatae = new uint8_t *[Interfaces.size()];
    for (size_t i = 0; i < Interfaces.size(); i++)
    {
        std::unique_ptr<Socket> Injector;
        Injector.reset(new Socket());
        Injector->CreateIllegal(Interfaces[i].c_str());
        PacketVideo[i] = new uint8_t[HeaderSize];
        PacketDatae[i] = new uint8_t[HeaderSize];
        memcpy(InjectPacketLLC.DeviceInfo_SRC, Injector->InterfaceMACGet(), 6);

        int offset = 0;
        memcpy(PacketVideo[i] + offset, InjectPacketLLC.RadioHeader, sizeof(InjectPacketLLC.RadioHeader));
        offset += sizeof(InjectPacketLLC.RadioHeader);
        memcpy(PacketVideo[i] + offset, InjectPacketLLC.RadioInfo, sizeof(InjectPacketLLC.RadioInfo));
        offset += sizeof(InjectPacketLLC.RadioInfo);
        memcpy(PacketVideo[i] + offset, InjectPacketLLC.Data80211Info, sizeof(InjectPacketLLC.Data80211Info));
        offset += sizeof(InjectPacketLLC.Data80211Info);
        memcpy(PacketVideo[i] + offset, InjectPacketLLC.DeviceInfo_DST, sizeof(InjectPacketLLC.DeviceInfo_DST));
        offset += sizeof(InjectPacketLLC.DeviceInfo_DST);
        memcpy(PacketVideo[i] + offset, InjectPacketLLC.DeviceInfo_BSS, sizeof(InjectPacketLLC.DeviceInfo_BSS));
        offset += sizeof(InjectPacketLLC.DeviceInfo_BSS);
        memcpy(PacketVideo[i] + offset, InjectPacketLLC.DeviceInfo_SRC, sizeof(InjectPacketLLC.DeviceInfo_SRC));

        offset += sizeof(InjectPacketLLC.DeviceInfo_SRC);
        memcpy(PacketDatae[i], PacketVideo[i], offset);
        memcpy(PacketDatae[i] + offset, InjectPacketLLC.SquenceAndLLCDataE, sizeof(InjectPacketLLC.SquenceAndLLCDataE));
        memcpy(PacketVideo[i] + offset, InjectPacketLLC.SquenceAndLLCVideo, sizeof(InjectPacketLLC.SquenceAndLLCVideo));

        SocketInjectors.push_back(std::move(Injector));
    }
}

WIFIBroadCast::WIFICastDriver::~WIFICastDriver()
{
    for (int i = 0; i < InterfaceCount; ++i)
    {
        delete[] PacketVideo[i];
        delete[] PacketDatae[i];
    }
    delete[] PacketVideo;
    delete[] PacketDatae;

    for (size_t i = 0; i < SocketInjectors.size(); i++)
    {
        SocketInjectors[i].reset();
    }
}

void WIFIBroadCast::WIFICastDriver::WIFICastInject(uint8_t *data, int len, int InterfaceID, BroadCastType type, int delayUS)
{
    if (len > (SocketMTU - HeaderSize))
    {
        float PacketSize = (((float)len / (float)(SocketMTU - HeaderSize)) == ((int)(len / (SocketMTU - HeaderSize))))
                               ? ((int)(len / (SocketMTU - HeaderSize)))
                               : ((int)(len / (SocketMTU - HeaderSize))) + 1;
        for (size_t i = 0; i < PacketSize; i++)
        {
            uint8_t tmpData[SocketMTU] = {0x00};
            if (type == BroadCastType::VideoStream)
                memcpy(tmpData, PacketVideo[InterfaceID], HeaderSize);
            if (type == BroadCastType::DataStream)
                memcpy(tmpData, PacketDatae[InterfaceID], HeaderSize);

            if (!(((float)len / (float)(SocketMTU - HeaderSize)) == ((int)(len / (SocketMTU - HeaderSize)))) && i == (PacketSize - 1))
            {
                int size = ((SocketMTU - HeaderSize) - (PacketSize * (SocketMTU - HeaderSize) - len));

                tmpData[((size + HeaderSize))] = FrameCounter;
                memcpy(tmpData + HeaderSize, (data + (i * (SocketMTU - HeaderSize))), (size + HeaderSize));
                SocketInjectors[InterfaceID]->Inject(tmpData, (size + HeaderSize + 1));
            }
            else
            {
                tmpData[(SocketMTU)] = FrameCounter;
                memcpy(tmpData + HeaderSize, (data + (i * (SocketMTU - HeaderSize))), (SocketMTU - HeaderSize));
                SocketInjectors[InterfaceID]->Inject(tmpData, SocketMTU + 1);
            }
            if (delayUS)
                usleep(delayUS);

            FrameCounter++;
            if (FrameCounter >= 0xff)
                FrameCounter = 0;
        }
    }
    else
    {
        SocketInjectors[InterfaceID]->Inject(data, len);
    }
}