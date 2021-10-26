#include <string.h>
#ifdef DBEUG
#include <iostream>
#endif
#include <linux/v4l2-controls.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include "./SimplePictureProcessing.h"
#include "./thermometry.h"

#define XthermControl_Shutter 0x8000
#define XthermControl_ENVShutter 0x8001
#define XthermControl_OriginOuput 0x8002
#define XthermControl_16BitNUCOutput 0x8004
#define XthermControl_YUYVDataOuput 0x8005
#define XthermControl_NormalOuput 0x8020
#define XthermControl_ExtendNormalOutput 0x8021
#define XthermControl_ParaSaving 0x80FE
#define XthermControl_TempParaSaving 0x80FF
#define XthermControl_ColorPanelOutoput(x) (0x8800 | x) // XthermControl_ColorPanelOutoput 00-07

int v4l2Control(int CameraFD, int value)
{
    v4l2_control ctrl;
    ctrl.id = V4L2_CID_ZOOM_ABSOLUTE;
    ctrl.value = value;
    if (ioctl(CameraFD, VIDIOC_S_CTRL, &ctrl) == -1)
    {
        printf("v4l2_control error\n");
        return 0;
    }
    return 1;
}

struct XthermSDKOption
{
public:
    int IMGWidth = 384;
    int IMGHeight = 292;
    int CamMode = 4;
    friend void XthermSDKReInit(XthermSDKOption &xthermSDKOption, unsigned char *IMGData);
    friend void XthermSDKDataGet(XthermSDKOption &xthermSDKOption, unsigned char *IMGData, unsigned char *IMGOut, float *TempData);
    friend void XthermSDKDataParser(XthermSDKOption &xthermSDKOption, unsigned char *IMG);
    friend void XthermSDKDataReCalc(XthermSDKOption &xthermSDKOption, unsigned char *IMG);

    float Temp_Shutter;
    float Temp_Device;
    float Temp_FPA;
    float Temp_Offset;
    float Temp_Ref;
    float Temp_Air;
    float Env_Wet;
    float Env_Power;
    float Env_CamLens = 130;
    int Env_TempRange = 120;
    unsigned short Env_Distance;

private:
    size_t **midVar;
    char cameraSoftVersion[16];
    float Temp_Table[16384];
    float Env_ShutterOffset = 0;
    int CamDataOffset = 0;
    unsigned short *Env_CamSN;
    unsigned short *Env_CamVer;
    unsigned short Temp_Shutter_Raw;
    unsigned short Temp_Device_Raw;
};

void XthermSDKReInit(XthermSDKOption &xthermSDKOption, unsigned char *IMG)
{
    if (xthermSDKOption.CamMode == 4)
    {
        xthermSDKOption.midVar = (size_t **)calloc(7, sizeof(size_t *));
        SimplePictureProcessingInit(xthermSDKOption.IMGWidth, xthermSDKOption.IMGHeight - 4);
        SetParameter(100, 0.5f, 0.1f, 0.1f, 1.0f, 3.5f);
        SimplePictureProcessingInitMidVar(xthermSDKOption.midVar);
    }
    switch (xthermSDKOption.IMGWidth)
    {
    case 384:
        xthermSDKOption.CamDataOffset = xthermSDKOption.IMGWidth * (4 - 1);
        break;
    case 240:
        xthermSDKOption.CamDataOffset = xthermSDKOption.IMGWidth * (4 - 3);
        break;
    case 256:
        xthermSDKOption.CamDataOffset = xthermSDKOption.IMGWidth * (4 - 3);
        break;
    case 640:
        xthermSDKOption.CamDataOffset = xthermSDKOption.IMGWidth * (4 - 1);
        break;
    default:
    {
        throw -2;
    }
    break;
    }
    unsigned short *CamDataParse = (unsigned short *)IMG + xthermSDKOption.IMGWidth * (xthermSDKOption.IMGHeight - 4);
    memcpy(&xthermSDKOption.Temp_Shutter_Raw, CamDataParse + xthermSDKOption.CamDataOffset + 1, sizeof(unsigned short));
#ifdef DEBUG
    std::cout << "CAMDataInfo:"
              << "\n";
    std::cout << "  Shutter_Temp:" << xthermSDKOption.Temp_Shutter_Raw / 10.0f - 273.15f << "\n";
#endif
    memcpy(&xthermSDKOption.Temp_Device_Raw, CamDataParse + xthermSDKOption.CamDataOffset + 2, sizeof(unsigned short));
#ifdef DEBUG
    std::cout << "  Core_Temp   :" << xthermSDKOption.Temp_Device_Raw / 10.0f - 273.15f << "\n";
#endif
    memcpy(&xthermSDKOption.Temp_Offset, CamDataParse + xthermSDKOption.CamDataOffset + 127, sizeof(float));
    memcpy(&xthermSDKOption.Temp_Ref, CamDataParse + xthermSDKOption.CamDataOffset + 127 + 2, sizeof(float));
    memcpy(&xthermSDKOption.Temp_Air, CamDataParse + xthermSDKOption.CamDataOffset + 127 + 4, sizeof(float));
    memcpy(&xthermSDKOption.Env_Wet, CamDataParse + xthermSDKOption.CamDataOffset + 127 + 6, sizeof(float));
    memcpy(&xthermSDKOption.Env_Power, CamDataParse + xthermSDKOption.CamDataOffset + 127 + 8, sizeof(float));
    memcpy(&xthermSDKOption.Env_Distance, CamDataParse + xthermSDKOption.CamDataOffset + 127 + 10, sizeof(unsigned short));
    memcpy((unsigned short *)xthermSDKOption.cameraSoftVersion, CamDataParse + xthermSDKOption.CamDataOffset + 24, 16 * sizeof(uint8_t));
#ifdef DEBUG
    std::cout << "  SNVersion   :" << xthermSDKOption.cameraSoftVersion << "\n";
#endif
    thermometryT4Line(xthermSDKOption.IMGWidth, xthermSDKOption.IMGHeight,
                      xthermSDKOption.Temp_Table,
                      CamDataParse,
                      &xthermSDKOption.Temp_FPA,
                      &xthermSDKOption.Temp_Offset,
                      &xthermSDKOption.Temp_Ref,
                      &xthermSDKOption.Temp_Air,
                      &xthermSDKOption.Env_Wet,
                      &xthermSDKOption.Env_Power,
                      (unsigned short *)&xthermSDKOption.Env_Distance,
                      xthermSDKOption.Env_CamLens,
                      xthermSDKOption.Env_ShutterOffset,
                      xthermSDKOption.Env_TempRange);
#ifdef DEBUG
    std::cout << "  FPA_Temp  :" << xthermSDKOption.Temp_FPA << "\n";
#endif

    xthermSDKOption.Temp_Shutter = (float)xthermSDKOption.Temp_Shutter_Raw / 10.0f - 273.15f;
    xthermSDKOption.Temp_Device = (float)xthermSDKOption.Temp_Device_Raw / 10.0f - 273.15f;
};

void XthermSDKDataParser(XthermSDKOption &xthermSDKOption, unsigned char *IMG)
{
    unsigned short *CamDataParse = (unsigned short *)IMG + xthermSDKOption.IMGWidth * (xthermSDKOption.IMGHeight - 4);
    memcpy(&xthermSDKOption.Temp_Shutter_Raw, CamDataParse + xthermSDKOption.CamDataOffset + 1, sizeof(unsigned short));
    memcpy(&xthermSDKOption.Temp_Device_Raw, CamDataParse + xthermSDKOption.CamDataOffset + 2, sizeof(unsigned short));
    memcpy(&xthermSDKOption.Temp_Offset, CamDataParse + xthermSDKOption.CamDataOffset + 127, sizeof(float));
    memcpy(&xthermSDKOption.Temp_Ref, CamDataParse + xthermSDKOption.CamDataOffset + 127 + 2, sizeof(float));
    memcpy(&xthermSDKOption.Temp_Air, CamDataParse + xthermSDKOption.CamDataOffset + 127 + 4, sizeof(float));
    memcpy(&xthermSDKOption.Env_Wet, CamDataParse + xthermSDKOption.CamDataOffset + 127 + 6, sizeof(float));
    memcpy(&xthermSDKOption.Env_Power, CamDataParse + xthermSDKOption.CamDataOffset + 127 + 8, sizeof(float));
    memcpy(&xthermSDKOption.Env_Distance, CamDataParse + xthermSDKOption.CamDataOffset + 127 + 10, sizeof(unsigned short));
    memcpy((unsigned short *)xthermSDKOption.cameraSoftVersion, CamDataParse + xthermSDKOption.CamDataOffset + 24, 16 * sizeof(uint8_t));
    xthermSDKOption.Temp_Shutter = (float)xthermSDKOption.Temp_Shutter_Raw / 10.0f - 273.15f;
    xthermSDKOption.Temp_Device = (float)xthermSDKOption.Temp_Device_Raw / 10.0f - 273.15f;
}

void XthermSDKDataReCalc(XthermSDKOption &xthermSDKOption, unsigned char *IMG)
{
    unsigned short *CamDataParse = (unsigned short *)IMG + xthermSDKOption.IMGWidth * (xthermSDKOption.IMGHeight - 4);
    thermometryT4Line(xthermSDKOption.IMGWidth, xthermSDKOption.IMGHeight,
                      xthermSDKOption.Temp_Table,
                      CamDataParse,
                      &xthermSDKOption.Temp_FPA,
                      &xthermSDKOption.Temp_Offset,
                      &xthermSDKOption.Temp_Ref,
                      &xthermSDKOption.Temp_Air,
                      &xthermSDKOption.Env_Wet,
                      &xthermSDKOption.Env_Power,
                      (unsigned short *)&xthermSDKOption.Env_Distance,
                      xthermSDKOption.Env_CamLens,
                      xthermSDKOption.Env_ShutterOffset,
                      xthermSDKOption.Env_TempRange);
    xthermSDKOption.Temp_Shutter = (float)xthermSDKOption.Temp_Shutter_Raw / 10.0f - 273.15f;
    xthermSDKOption.Temp_Device = (float)xthermSDKOption.Temp_Device_Raw / 10.0f - 273.15f;
}

void XthermSDKDataGet(XthermSDKOption &xthermSDKOption, unsigned char *IMGData, unsigned char *IMGOut, float *TempData)
{
    thermometrySearch(xthermSDKOption.IMGWidth, xthermSDKOption.IMGHeight, xthermSDKOption.Temp_Table,
                      (unsigned short *)IMGData, TempData, xthermSDKOption.Env_TempRange, xthermSDKOption.CamMode);
    if (xthermSDKOption.CamMode == 4)
        Compute((unsigned short *)IMGData, IMGOut, 2, xthermSDKOption.midVar);
    xthermSDKOption.Temp_Shutter = (float)xthermSDKOption.Temp_Shutter_Raw / 10.0f - 273.15f;
    xthermSDKOption.Temp_Device = (float)xthermSDKOption.Temp_Device_Raw / 10.0f - 273.15f;
}

void XthermSDKUserTempSet(int X, int Y, int index, int CameraFD)
{
    v4l2_control crtTmp;
    int x, y;
    switch (index)
    {
    case 0:
        x = 0xf000 + X;
        y = 0xf200 + Y;
        break;
    case 1:
        x = 0xf400 + X;
        y = 0xf600 + Y;
        break;
    case 2:
        x = 0xf800 + X;
        y = 0xfa00 + Y;
        break;
    }

    crtTmp.id = V4L2_CID_ZOOM_ABSOLUTE;
    crtTmp.value = x;
    ioctl(CameraFD, VIDIOC_S_CTRL, &crtTmp);
    crtTmp.value = y;
    ioctl(CameraFD, VIDIOC_S_CTRL, &crtTmp);
}

float *XthermSDKTempDataInit(int width, int height)
{
    return (float *)calloc(width * (height - 4) + 10, sizeof(float));
}

unsigned char *XthermSDKIMGDataInit(int width, int height)
{
    return (unsigned char *)calloc(width * height * 4, sizeof(char));
}
//=========================================================================================//
void sendFloatCommand(int CameraFD, int position, unsigned char value0, unsigned char value1, unsigned char value2, unsigned char value3, int interval0,
                      int interval1, int interval2, int interval3, int interval4)
{
    int psitionAndValue0 = (position << 8) | (0x000000ff & value0);
    // printf("psitionAndValue0:%X\n", psitionAndValue0);
    if (v4l2Control(CameraFD, psitionAndValue0) == 0)
    {
        // printf("control fail psitionAndValue0~~\n");
        // exit(EXIT_FAILURE);
    }
    int psitionAndValue1 = ((position + 1) << 8) | (0x000000ff & value1);
    // printf("psitionAndValue1:%X\n", psitionAndValue1);
    if (v4l2Control(CameraFD, psitionAndValue1) == 0)
    {
        // printf("control fail psitionAndValue1~~\n");
        // exit(EXIT_FAILURE);
    }
    int psitionAndValue2 = ((position + 2) << 8) | (0x000000ff & value2);
    // printf("psitionAndValue2:%X\n", psitionAndValue2);
    if (v4l2Control(CameraFD, psitionAndValue2) == 0)
    {
        // printf("control fail psitionAndValue2~~\n");
        // exit(EXIT_FAILURE);
    }
    int psitionAndValue3 = ((position + 3) << 8) | (0x000000ff & value3);
    // printf("psitionAndValue3:%X\n", psitionAndValue3);
    if (v4l2Control(CameraFD, psitionAndValue3) == 0)
    {
        // printf("control fail psitionAndValue3~~\n");
        // exit(EXIT_FAILURE);
    }
}

void sendUshortCommand(int CameraFD, int position, unsigned char value0, unsigned char value1)
{
    int psitionAndValue0 = (position << 8) | (0x000000ff & value0);
    // printf("psitionAndValue0:%X\n", psitionAndValue0);
    if (v4l2Control(CameraFD, psitionAndValue0) == 0)
    {
        // printf("control fail psitionAndValue0~~\n");
        // exit(EXIT_FAILURE);
    }
    int psitionAndValue1 = ((position + 1) << 8) | (0x000000ff & value1);
    // printf("psitionAndValue1:%X\n", psitionAndValue1);
    if (v4l2Control(CameraFD, psitionAndValue1) == 0)
    {
        // printf("control fail psitionAndValue1~~\n");
        // exit(EXIT_FAILURE);
    }
}

void sendByteCommand(int CameraFD, int position, unsigned char value0, int interval0)
{
    int psitionAndValue0 = (position << 8) | (0x000000ff & value0);
    v4l2Control(CameraFD, psitionAndValue0);
}
//温度偏移
void sendCorrection(int CameraFD, float correction)
{
    unsigned char iputCo[4];
    memcpy(iputCo, &correction, sizeof(float));
    sendFloatCommand(CameraFD, 0 * 4, iputCo[0], iputCo[1], iputCo[2], iputCo[3], 20, 40, 60, 80, 120);
    // printf("sendCorrection 0:%d,1:%d,2:%d,3:%d\n", iputCo[0], iputCo[1], iputCo[2], iputCo[3]);
}
//反射温度
void sendReflection(int CameraFD, float reflection)
{
    unsigned char iputRe[4];
    memcpy(iputRe, &reflection, sizeof(float));
    sendFloatCommand(CameraFD, 1 * 4, iputRe[0], iputRe[1], iputRe[2], iputRe[3], 20, 40, 60, 80, 120);
}
//环境温度
void sendAmb(int CameraFD, float amb)
{
    unsigned char iputAm[4];
    memcpy(iputAm, &amb, sizeof(float));
    sendFloatCommand(CameraFD, 2 * 4, iputAm[0], iputAm[1], iputAm[2], iputAm[3], 20, 40, 60, 80, 120);
}
//环境湿度
void sendHumidity(int CameraFD, float humidity)
{
    unsigned char iputHu[4];
    memcpy(iputHu, &humidity, sizeof(float));
    sendFloatCommand(CameraFD, 3 * 4, iputHu[0], iputHu[1], iputHu[2], iputHu[3], 20, 40, 60, 80, 120);
}
//发射率
void sendEmissivity(int CameraFD, float emiss)
{
    unsigned char iputEm[4];
    memcpy(iputEm, &emiss, sizeof(float));
    sendFloatCommand(CameraFD, 4 * 4, iputEm[0], iputEm[1], iputEm[2], iputEm[3], 20, 40, 60, 80, 120);
}
//距离
void sendDistance(int CameraFD, unsigned short distance)
{
    unsigned char iputDi[2];
    memcpy(iputDi, &distance, sizeof(unsigned short));
    sendUshortCommand(CameraFD, 5 * 4, iputDi[0], iputDi[1]);
}
//保存参数
void savePara(int CameraFD)
{
    v4l2Control(CameraFD, 0x80ff);
}