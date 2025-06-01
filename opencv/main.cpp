#include <iostream>
#include <random>

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

constexpr int FILTER_KERNEL_SIZE = 9;
constexpr float LARGE_FLOAT = 2048.f;
constexpr size_t QUEUE_MAX_SIZE = 32;
constexpr size_t THRESHOLD_MIN = 256;
constexpr size_t THRESHOLD_MAX = 2048;
constexpr char VIDEO_PATH[] = "/home/beak/wksp/learn/opencv/media/output.mp4";


size_t random_index(const size_t max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, max);
    return dist(gen);
}

void filter(const cv::Mat &src, cv::Mat &dst) {
    static const cv::Mat kernel =
            cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(FILTER_KERNEL_SIZE, FILTER_KERNEL_SIZE));

    cv::morphologyEx(src, dst, cv::MORPH_OPEN, kernel);
}

bool retrack(cv::Ptr<cv::TrackerCSRT> &tracker, const cv::Mat &diff) {
    static std::vector<std::vector<cv::Point> > contours;
    static std::vector<cv::Rect> objects;

    objects.clear();
    cv::findContours(diff, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    for (auto &contour: contours) {
        const cv::Rect rect = cv::boundingRect(contour);
        if (const int area = rect.area(); area >= THRESHOLD_MIN && area <= THRESHOLD_MAX) {
            objects.push_back(rect);
        }
    }

    if (!objects.empty()) {
        const cv::Rect &obj = objects[random_index(objects.size() - 1)];
        tracker = cv::TrackerCSRT::create();
        tracker->init(diff, obj);
        return true;
    }
    return false;
}

int main() {
    cv::VideoCapture cap(VIDEO_PATH);
    if (!cap.isOpened()) {
        std::cerr << "Failed to open: " << VIDEO_PATH << std::endl;
        return -1;
    }

    cv::Rect roi;
    cv::Rect bbox;
    cv::Mat debug;
    cv::Mat lastGrayRoi, frame, grayRoi, diff;
    cv::Ptr<cv::TrackerCSRT> tracker;
    bool isTracking = false;

    while (cap.read(frame)) {
        if (roi.empty()) {
            roi = cv::selectROI("Select ROI", frame, false);
            cv::destroyWindow("Select ROI");

            cv::cvtColor(frame(roi), grayRoi, cv::COLOR_BGR2GRAY);
            swap(grayRoi, lastGrayRoi);
            continue;
        }

        cv::cvtColor(frame(roi), grayRoi, cv::COLOR_BGR2GRAY);
        // cv::GaussianBlur(grayRoi, grayRoi, cv::Size(5, 5), 0, 0);
        cv::absdiff(lastGrayRoi, grayRoi, diff);
        cv::threshold(diff, diff, 48, 255, cv::THRESH_BINARY);
        filter(diff, diff);

        if (isTracking) {
            if (!tracker->update(diff, bbox)) {
                std::cout << "Miss!" << std::endl;
                isTracking = retrack(tracker, diff);
            }
        } else {
            isTracking = retrack(tracker, diff);
        }

        if (isTracking) {
            static int i = 0;
            debug = grayRoi.clone();
            cv::rectangle(debug, bbox, 255, 2);
            std::cout << ++i << std::endl;
        } else {
            debug = grayRoi;
        }

        cv::imshow("track", debug);
        cv::imshow("diff", diff);
        swap(grayRoi, lastGrayRoi);
        if (cv::waitKey(isTracking ? 100 : 1) == 27)
            break;
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}
