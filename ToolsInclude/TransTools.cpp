#include "M_Tools.hpp"

unsigned char *Base::TransTools::MatToByte(cv::Mat imFrame, unsigned char *&imData, int &imRows, int &imCols, int &imSize, int &imTrueSize, int &imType)
{
    imRows = imFrame.rows;
    imCols = imFrame.cols;
    imTrueSize = imFrame.total() * imFrame.elemSize();
    imSize = imFrame.rows * imFrame.cols;
    imType = imFrame.type();
    imData = new unsigned char[imSize];
    imData = imFrame.data;
    return imData;
}

//If you need to get img info , please run  Base::ansTools::MatToByte()
cv::Mat Base::TransTools::ByteToMat(unsigned char *imByte, int imRows, int imCols, int imType)
{
    cv::Mat imData = cv::Mat(imRows, imCols, imType, imByte);
    return imData;
}

unsigned char *Base::TransTools::StringToByte(std::string strData)
{
    unsigned char *tmpByte = (unsigned char *)strData.c_str();
    return tmpByte;
}

std::string Base::TransTools::ByteToString(unsigned char *byteData)
{
    std::string tmpStr;
    char *tmpChar = (char *)byteData;
    tmpStr = (char *)tmpChar;
    return tmpStr;
}