#include <iostream>
#include <opencv2/opencv.hpp>
#include "tracker.h"


static constexpr float LARGE_FLOAT = 2048.f;
static constexpr bool DEBUG = true;
static constexpr char VIDEO_PATH[] = "/home/beak/wksp/learn/opencv/media/output.mp4";
static constexpr int FILTER_KERNEL_SIZE = 9;

constexpr int sqrt_int(int x) {
    int rt = 0;
    while(rt * rt < x) ++rt;
    return rt;
}

void filter(const cv::Mat &src, cv::Mat &dst) {
    static const cv::Mat kernel =
            cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(FILTER_KERNEL_SIZE, FILTER_KERNEL_SIZE));

    cv::morphologyEx(src, dst, cv::MORPH_OPEN, kernel);
}

int main() {
    cv::VideoWriter writer;
    cv::VideoCapture cap(VIDEO_PATH);
    if (!cap.isOpened()) {
        std::cerr << "Failed to open: " << VIDEO_PATH << std::endl;
        return -1;
    }

    cv::Rect roi, bbox;
    cv::Mat frame, gray, mask, debug;
    const auto bs = cv::createBackgroundSubtractorKNN();
    Tracker tracker;
    bool isTracking = false;

    while (cap.read(frame)) {
        if (DEBUG && roi.empty()) {
            roi = cv::selectROI("Select ROI", frame, false);
            cv::destroyWindow("Select ROI");
            continue;
        }

        cv::cvtColor(frame(roi), gray, cv::COLOR_BGR2GRAY);
        // cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0, 0);
        bs->apply(gray, mask);
        filter(mask, mask);

        const auto result = tracker.apply(mask, bbox);
        if (result == Tracker::Result::TRACKED) {
            isTracking = true;
            std::cout << "Tracked!" << std::endl;
        } else if (result == Tracker::Result::CHANGED) {
            std::cout << "Changed!" << std::endl;
        } else {
            if (isTracking) {
                isTracking = false;
                std::cout << "Miss!" << std::endl;
            }
        }

        if constexpr (DEBUG) {
            constexpr int MIN_SIZE = sqrt_int(THRESHOLD_AREA_MIN) * 3;
            constexpr int MAX_SIZE = sqrt_int(THRESHOLD_AREA_MAX) * 3;
            const cv::Rect MIN_RECT(0, 0, MIN_SIZE, MIN_SIZE);
            const cv::Rect MAX_RECT(0, 0, MAX_SIZE, MAX_SIZE);
            cv::rectangle(gray, MIN_RECT, 255, 2);
            cv::rectangle(gray, MAX_RECT, 255, 2);
            if (isTracking){
                cv::rectangle(gray, bbox, 255, 2);
            }


            cv::hconcat(gray, mask, debug);
            cv::imshow("Debug", debug);
            if (cv::waitKey(isTracking ? 500 : 1) == 27)
                break;
        }
    }

    writer.release();
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
