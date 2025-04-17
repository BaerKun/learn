#include <iostream>

#include <opencv2/opencv.hpp>

constexpr int OPEN_KERNAL_SIZE = 5;
constexpr int CLOASE_KERNAL_SIZE = 9;
constexpr float LARGE_FLOAT = 2048.f;
constexpr size_t QUEUE_MAX_SIZE = 32;
constexpr size_t THRESHOLD_MIN = 40;
constexpr size_t THRESHOLD_MAX = 256;
constexpr char VIDEO_PATH[] = "/Users/bearkun/wksp/learn/opencv/media/video1.mp4";

void filter(const cv::Mat &src, cv::Mat &dst) {
    static const cv::Mat openKernal =
            cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(OPEN_KERNAL_SIZE, OPEN_KERNAL_SIZE));
    static const cv::Mat erodeKernal =
            cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(CLOASE_KERNAL_SIZE, CLOASE_KERNAL_SIZE));

//    cv::morphologyEx(src, dst, cv::MORPH_OPEN, openKernal);
    cv::dilate(dst, dst, erodeKernal);
}

template<typename Tp_ = float>
class PointQueue {
public:
    using InputArray = std::vector<cv::Point_<Tp_>>;

    explicit PointQueue(const size_t capacity) : points(capacity), rear(0), size(0), capacity(capacity) {
    }

    void emplace(const Tp_ x, const Tp_ y) {
        points[rear].x = x;
        points[rear].y = y;
        if (++rear == capacity)
            rear = 0;
        if (size < capacity)
            ++size;
    }

    [[nodiscard]] bool ready() const {
        return size == capacity;
    }

    [[nodiscard]] const InputArray &getInputArray() const {
        return points;
    }

private:
    InputArray points;
    size_t rear;
    size_t size;
    size_t capacity;
};

#include <vector>
#include <cmath>
#include <random>
#include <limits>

// 计算点到直线的垂直距离
float pointLineDistance(const cv::Point2f &pt, const cv::Point2f &dir, const cv::Point2f &pt_on_line) {
    cv::Point2f diff = pt - pt_on_line;
    return std::abs(diff.x * dir.y - diff.y * dir.x) / std::sqrt(dir.x * dir.x + dir.y * dir.y);
}

// RANSAC 拟合直线
cv::Vec4f ransacFitLine(const std::vector<cv::Point2f> &points,
                        int iterations = 100,
                        float threshold = 1.0f,
                        int min_inliers = 8) {
    int best_inliers = 0;
    cv::Vec4f best_model;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, points.size() - 1);

    for (int i = 0; i < iterations; ++i) {
        // 随机取两个不同的点
        int idx1 = dis(gen), idx2 = dis(gen);
        if (idx1 == idx2) continue;

        const cv::Point2f &p1 = points[idx1];
        const cv::Point2f &p2 = points[idx2];
        cv::Point2f dir = p2 - p1;

        // 跳过太短的线
        if (cv::norm(dir) < 1e-6) continue;

        int inlier_count = 0;
        std::vector<cv::Point2f> inliers;

        for (const auto &pt: points) {
            float dist = pointLineDistance(pt, dir, p1);
            if (dist < threshold) {
                inliers.push_back(pt);
                ++inlier_count;
            }
        }

        if (inlier_count > best_inliers && inlier_count >= min_inliers) {
            best_inliers = inlier_count;
            // 用内点重新精确拟合
            cv::fitLine(inliers, best_model, cv::DIST_L2, 0, 0.01, 0.01);
        }
    }

    return best_model; // [vx, vy, x0, y0]
}


int main() {
    cv::VideoCapture cap(VIDEO_PATH);
    if (!cap.isOpened()) {
        std::cerr << "Failed to open: " << VIDEO_PATH << std::endl;
        return -1;
    }

    cv::Mat debug;
    cv::Mat lastGrayFrame, frame, gray, diff;
    std::vector<std::vector<cv::Point>> contours;
    PointQueue queue(QUEUE_MAX_SIZE);
    cv::Vec4f line;
    while (cap.read(frame)) {
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        if (lastGrayFrame.empty()) {
            swap(gray, lastGrayFrame);
            continue;
        }

        cv::absdiff(lastGrayFrame, gray, diff);
        cv::threshold(diff, diff, 16, 255, cv::THRESH_BINARY);
        filter(diff, diff);
        swap(gray, lastGrayFrame);

        cv::findContours(diff, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        for (auto &contour: contours) {
            if (contour.size() >= THRESHOLD_MIN && contour.size() <= THRESHOLD_MAX) {
                cv::Rect rect = cv::boundingRect(contour);
                queue.emplace((float) rect.x + (float) rect.width / 2.f, (float) rect.y + (float) rect.height / 2.f);

                cv::rectangle(diff, rect, 255);
            }
        }

        if (debug.empty()) {
            debug = cv::Mat(frame.size(), CV_8UC1);
        }

        debug.setTo(0);
        for (const auto &point: queue.getInputArray()) {
            cv::circle(debug, point, 3, 255);
        }

        if (queue.ready()) {
            line = ransacFitLine(queue.getInputArray());
            printf("line: %f, %f, %f, %f\n", line[0], line[1], line[2], line[3]);
//            cv::fitLine(queue.getInputArray(), line, cv::DIST_L2, 0, 0.01, 0.01);
            const cv::Point2f p1(line[2] - line[0] * LARGE_FLOAT, line[3] - line[1] * LARGE_FLOAT);
            const cv::Point2f p2(line[2] + line[0] * LARGE_FLOAT, line[3] + line[1] * LARGE_FLOAT);

            cv::line(debug, p1, p2, 255, 2);
        }

        cv::imshow("diff", diff);
        cv::imshow("debug", debug);
        const int key = cv::waitKey(50);
        if (key == 27) {
            break;
        }
    }
    cap.release();

    return 0;
}