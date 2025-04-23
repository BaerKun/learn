#include <random>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

static constexpr int THRESHOLD_AREA_MIN = 225;
static constexpr int THRESHOLD_AREA_MAX = 2048;
static constexpr int THRESHOLD_DISP_MAG_MIN = 10; // Displacement Magnitude 位移大小

class Tracker {
public:
    enum class Result{LOST, TRACKED, CHANGED};

    Tracker() = default;
    ~Tracker() = default;

    Result apply(const cv::Mat &frame, cv::Rect &bbox){
        if (!isTracking_){
            isTracking_ = retrack(frame, bbox);
            return isTracking_ ? Result::TRACKED : Result::LOST;
        }
        if (!update(frame, bbox)){
            isTracking_ = retrack(frame, bbox);
            return isTracking_ ? Result::CHANGED : Result::LOST;
        }
        return Result::TRACKED;
    }

private:
    static size_t random_index(const size_t max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dist(0, max);
        return dist(gen);
    }

    bool retrack(const cv::Mat &frame, cv::Rect &bbox) {
        static std::vector<std::vector<cv::Point> > contours;
        static std::vector<cv::Rect> objects;
    
        objects.clear();
        cv::findContours(frame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        for (auto &contour: contours) {
            const cv::Rect rect = cv::boundingRect(contour);
            if (const int area = rect.area(); area >= THRESHOLD_AREA_MIN && area <= THRESHOLD_AREA_MAX) {
                objects.push_back(rect);
            }
        }
    
        if (!objects.empty()) {
            const cv::Rect &obj = objects[random_index(objects.size() - 1)];
            bbox.x = std::max(obj.x - obj.width, 0);
            bbox.y = std::max(obj.y - obj.height, 0);
            bbox.width = std::min(obj.x + obj.width * 2, frame.cols - 1) - bbox.x;
            bbox.height = std::min(obj.y + obj.height * 2, frame.rows - 1) - bbox.y;
            lastCenter_.x = bbox.x + bbox.width / 2;
            lastCenter_.y = bbox.y + bbox.height / 2;
    
            tracker_ = cv::TrackerCSRT::create();
            tracker_->init(frame, bbox);
            return true;
        }
        return false;
    }

    bool update(const cv::Mat &frame, cv::Rect &bbox) {
        if(!tracker_->update(frame, bbox)){
            return false;
        }

        cv::Point2i center{bbox.x + bbox.width / 2, bbox.y + bbox.height / 2};
        if (cv::norm(center - lastCenter_) < THRESHOLD_DISP_MAG_MIN) {
            return false;
        }
        
        lastCenter_ = center;
        return true;
    }

    cv::Ptr<cv::TrackerCSRT> tracker_;
    bool isTracking_ = false;
    cv::Point2i lastCenter_{};
};