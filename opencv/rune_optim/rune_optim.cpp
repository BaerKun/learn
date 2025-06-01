#include <cmath>
#include <iostream>
#include <vector>
#include <array>
#include <opencv2/opencv.hpp>

struct PeakRange {
    int start, width;
};

int main() {
    cv::Mat img = cv::imread("../rune.jpg");
    std::vector<cv::Mat> bgr;
    cv::split(img, bgr);

    cv::Mat bin;
    cv::resize(bgr[2], bin, cv::Size(128, 128), 0, 0, cv::INTER_LINEAR);
    cv::threshold(bin, bin, 200, 255, cv::THRESH_BINARY);
    const float scaleX = img.cols / 128.f;
    const float scaleY = img.rows / 128.f;
    cv::imshow("bin", bin);

    const double radius = sqrt(bin.rows * bin.rows + bin.cols * bin.cols) / 2;
    cv::Mat polar = cv::Mat::zeros(cv::Size((int) radius, 360), CV_8UC1);
    cv::warpPolar(bin, polar, cv::Size((int) radius, 360),
                  cv::Point(bin.cols / 2, bin.rows / 2), radius,
                  cv::WARP_POLAR_LINEAR);
    cv::imshow("polar", polar);

    cv::Mat half;
    cv::bitwise_and(polar.rowRange(0, polar.rows / 2), polar.rowRange(polar.rows / 2, polar.rows), half);
    cv::imshow("half", half);

    cv::Mat countNonZero;
    cv::reduce(half & 1, countNonZero, 1, cv::REDUCE_SUM, CV_32S);

    double minVal, maxVal;
    cv::minMaxLoc(countNonZero, &minVal, &maxVal);
    const int threshold = static_cast<int>(minVal + maxVal) / 2;

    cv::Mat mask = countNonZero > threshold;
    std::vector<PeakRange> peaks;
    int start = 0;
    int width = mask.at<uchar>(0) ? 1 : 0;
    for (int i = 1; i < countNonZero.rows; i++) {
        if (mask.at<uchar>(i)) {
            if (width++ == 0) {
                start = i;
            }
        } else {
            if (width != 0) {
                peaks.push_back({start, width});
            }
            width = 0;
        }
    }
    if (width != 0) {
        peaks.push_back({start, width});
    }

    /*
     * 合并首尾
     */

    if (peaks.size() < 2) {
        std::cerr << "Cannot find enough peaks." << std::endl;
        return -1;
    }

    std::sort(peaks.begin(), peaks.end(), [](const PeakRange &a, const PeakRange &b) { return a.width > b.width; });
    int first = peaks[0].start + peaks[0].width / 2;
    int second = peaks[1].start + peaks[1].width / 2;
    std::array<int, 4> theta = {
            first, second,
            first + 180, second + 180
    };

    const auto findRightEdge = [](const cv::Mat &mat, const int rowIdx) {
        const cv::Mat row = mat.row(rowIdx);
        for (int i = mat.cols - 1; i; --i) {
            if (row.at<uchar>(i))
                return i;
        }
        return 0;
    };

    std::array<cv::Point2f, 4> pts;
    for (int i = 0; i < 4; ++i) {
        const float rad = theta[i] * CV_PI / 180.f;
        const auto r = (float) findRightEdge(polar, theta[i]);
        pts[i] = cv::Point2f(img.cols / 2.f, img.rows / 2.f) +
                 cv::Point2f(cosf(rad) * scaleX, sinf(rad) * scaleY) * r;
    }

    for (const auto pt: pts) {
        cv::circle(img, pt, 2, cv::Scalar(255, 0, 0), -1);
    }
    cv::imshow("result", img);

    cv::Mat plot = cv::Mat::zeros(half.size(), CV_8UC1);
    for (int i = 1; i < countNonZero.rows; i++) {
        cv::line(plot, cv::Point(countNonZero.at<int>(i - 1), i - 1),
                 cv::Point(countNonZero.at<int>(i), i), cv::Scalar(255), 1);
    }
    cv::line(plot, cv::Point(threshold, 0), cv::Point(threshold, plot.rows), cv::Scalar(255), 1);
    cv::line(plot, cv::Point(0, first), cv::Point(plot.cols, first), cv::Scalar(255), 1);
    cv::line(plot, cv::Point(0, second), cv::Point(plot.cols, second), cv::Scalar(255), 1);
    cv::imshow("plot", plot);
    cv::waitKey();
}
