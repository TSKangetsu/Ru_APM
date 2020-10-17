#pragma once
#include <string.h>
#include <iostream>
#include <linux/v4l2-controls.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include "./SimplePictureProcessing.h"
#include "./thermometry.h"

struct XthermSDKOption
{
public:
    int IMGWidth = 384;
    int IMGHeight = 292;
    int CamMode = 4;
    friend void XthermSDKReInit(XthermSDKOption &xthermSDKOption, unsigned char *IMGData);
    friend void XthermSDKDataGet(XthermSDKOption &xthermSDKOption, unsigned char *IMGData, unsigned char *IMGOut, float *TempData);

private:
    size_t **midVar;
    char cameraSoftVersion[16];
    float Temp_Table[16384];
    unsigned short Temp_Shutter;
    unsigned short Temp_Device;
    float Temp_FPA;
    float Temp_Offset;
    float Temp_Ref;
    float Temp_Air;
    float Env_Wet;
    float Env_Power;
    unsigned short Env_Distance;
    float Env_CamLens = 130;
    float Env_ShutterOffset = 0;
    int Env_TempRange = 120;
    unsigned short *Env_CamSN;
    unsigned short *Env_CamVer;
};

inline void XthermSDKReInit(XthermSDKOption &xthermSDKOption, unsigned char *IMG)
{
    if (xthermSDKOption.CamMode == 4)
    {
        xthermSDKOption.midVar = (size_t **)calloc(7, sizeof(size_t *));
        SimplePictureProcessingInit(xthermSDKOption.IMGWidth, xthermSDKOption.IMGHeight - 4);
        SetParameter(100, 0.5f, 0.1f, 0.1f, 1.0f, 3.5f);
        SimplePictureProcessingInitMidVar(xthermSDKOption.midVar);
    }

    int CamDataOffset = xthermSDKOption.IMGWidth * (4 - 1);
    unsigned short *CamDataParse = (unsigned short *)IMG + xthermSDKOption.IMGWidth * (xthermSDKOption.IMGHeight - 4);
    memcpy(&xthermSDKOption.Temp_Shutter, CamDataParse + CamDataOffset + 1, sizeof(unsigned short));
#ifdef DEBUG
    std::cout << "CAMDataInfo:"
              << "\n";
    std::cout << "  Shutter_Temp:" << xthermSDKOption.Temp_Shutter / 10.0f - 273.15f << "\n";
#endif
    memcpy(&xthermSDKOption.Temp_Device, CamDataParse + CamDataOffset + 2, sizeof(unsigned short));
#ifdef DEBUG
    std::cout << "  Core_Temp   :" << xthermSDKOption.Temp_Device / 10.0f - 273.15f << "\n";
#endif
    memcpy(&xthermSDKOption.Temp_Offset, CamDataParse + CamDataOffset + 127, sizeof(float));
    memcpy(&xthermSDKOption.Temp_Ref, CamDataParse + CamDataOffset + 127 + 2, sizeof(float));
    memcpy(&xthermSDKOption.Temp_Air, CamDataParse + CamDataOffset + 127 + 4, sizeof(float));
    memcpy(&xthermSDKOption.Env_Wet, CamDataParse + CamDataOffset + 127 + 6, sizeof(float));
    memcpy(&xthermSDKOption.Env_Power, CamDataParse + CamDataOffset + 127 + 8, sizeof(float));
    memcpy(&xthermSDKOption.Env_Distance, CamDataParse + CamDataOffset + 127 + 10, sizeof(unsigned short));
    memcpy((unsigned short *)xthermSDKOption.cameraSoftVersion, CamDataParse + CamDataOffset + 24, 16 * sizeof(uint8_t));
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
};

inline void XthermSDKDataGet(XthermSDKOption &xthermSDKOption, unsigned char *IMGData, unsigned char *IMGOut, float *TempData)
{
    thermometrySearch(xthermSDKOption.IMGWidth, xthermSDKOption.IMGHeight, xthermSDKOption.Temp_Table,
                      (unsigned short *)IMGData, TempData, xthermSDKOption.Env_TempRange, xthermSDKOption.CamMode);
    if (xthermSDKOption.CamMode == 4)
        Compute((unsigned short *)IMGData, IMGOut, 2, xthermSDKOption.midVar);
}

inline void XthermSDKUserTempSet(int X, int Y, int index, int CameraFD)
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

inline float *XthermSDKTempDataInit(int width, int height)
{
    return (float *)calloc(width * (height - 4) + 10, sizeof(float));
}

inline unsigned char *XthermSDKIMGDataInit(int width, int height)
{
    return (unsigned char *)calloc(width * height * 4, sizeof(char));
}