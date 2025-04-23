#include "opencv2/opencv.hpp"

template<typename T>
struct ConstexprVector2 {
    T x, y;

    constexpr ConstexprVector2(const T x, const T y) : x(x), y(y) {
        // error
        // this->x = x;
        // this->y = y;
    }

    constexpr ConstexprVector2 operator+(const ConstexprVector2 &other)
    const /* 必须是const */ {
        return ConstexprVector2(this->x + other.x, this->y + other.y);
    }

    cv::Vec<T, 2> toCvVec() const {
        return {this->x, this->y};
    }
};

int main() {
    // error: 未定义constexpr构造函数
    // constexpr cv::Vec2i v{1, 2};
    // std::cout << v << std::endl;

    constexpr ConstexprVector2<int> p{1, 2};
    constexpr ConstexprVector2<int> q{3, 4};
    constexpr ConstexprVector2<int> r = p + q;
    const cv::Vec2i u = r.toCvVec();
    std::cout << u << std::endl;
}
