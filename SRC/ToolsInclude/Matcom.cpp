#include "M_Tools.hpp"

int Base::Matcom::ConvertSaveImg(std::string imName, cv::Mat imageData)
{
    //pass comfirm 7/11/19
    try
    {
        std::cout << "\033[35m[MatCom]convert save Image success!\033[0m\n";
        cv::imwrite(imName, imageData);
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cout << "\033[31m[MatCom]convert save Image failed : ";
        std::cout << e.what() << "\033[0m\n";
        return -1;
    }
}

cv::Mat Base::Matcom::ImageGet(std::string imdataName)
{
    //pass comfirm 7/11/19
    cv::Mat Image;
    try
    {
        std::cout << "\033[35m[MatCom]get image from disk success\033[0m\n";
        Image = cv::imread(imdataName, 1);
    }
    catch (const std::exception &e)
    {
        std::cout << "\033[31m[MatCom]get image from disk failed : ";
        std::cout << e.what() << "\033[31m\n";
    }
    return Image;
}

// int ImageDis::ImageDisPlay::ImageDisPlayFunc(int argc, std::string ins)
// {
//     //pass comfirm 7/11/19
//     //here just some MeanningLess function ,don't care
//     if (argc != 3)
//     {
//         printf("usage: DisplayImage.out <Image_Path>\n");
//         return -1;
//     }
//     cv::Mat image;
//     image = cv::imread(ins, 1);
//     if (!image.data)
//     {
//         printf("No image data \n");
//         return -1;
//     }
//     cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
//     imshow("Display Image", image);
//     cv::waitKey(0);
//     return 0;
// }
