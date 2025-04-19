#include "MvCameraControl.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
    MV_CC_DEVICE_INFO_LIST stDeviceList = {0};
    int nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
    if (nRet != MV_OK || stDeviceList.nDeviceNum == 0) {
        cout << "未发现相机！" << endl;
        return -1;
    }

    void* handle = nullptr;
    nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[0]);
    if (nRet != MV_OK) return -1;
    MV_CC_OpenDevice(handle, 0, 0);
    MV_CC_StartGrabbing(handle);

    MV_CC_SetExposureTime(handle, 20000);
    MV_CC_SetFrameRate(handle, 120);

    int frameWidth = 1440;
    int frameHeight = 1080;
    int fps = 30;
    VideoWriter writer("video.avi", VideoWriter::fourcc('M','J','P','G'), fps, Size(frameWidth, frameHeight), false);

    if (!writer.isOpened()) {
        cerr << "无法打开视频写入器！" << endl;
        return -1;
    }

    cout << "开始采集，按 'q' 退出..." << endl;

    while (true) {
        MV_FRAME_OUT stImage = {nullptr};
        nRet = MV_CC_GetImageBuffer(handle, &stImage, 1000);
        if (nRet == MV_OK) {
            int width = stImage.stFrameInfo.nWidth;
            int height = stImage.stFrameInfo.nHeight;

            Mat img(height, width, CV_8UC1, stImage.pBufAddr);
//            writer.write(img);
            imshow("Hik Camera", img);

            MV_CC_FreeImageBuffer(handle, &stImage);

            if (waitKey(1) == 'q') break;
        } else {
            cout << "获取图像失败" << endl;
        }
    }

    // 4. 清理资源
    MV_CC_StopGrabbing(handle);
    MV_CC_CloseDevice(handle);
    MV_CC_DestroyHandle(handle);
    writer.release();
    destroyAllWindows();

    cout << "采集结束，视频已保存为" << endl;
    return 0;
}
