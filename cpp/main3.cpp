#include <iostream>
#include <cmath>

// c++17
template<typename T>
T auto_sqrt(T x) {
    // 编译时判断，不生成其他分支
    if constexpr (std::is_same<T, float>::value) {
        return sqrtf(x);
    } else if constexpr (std::is_same_v<T, double>) {
        return sqrt(x);
    } else {
        return static_cast<T>(sqrt(static_cast<double>(x)));
    }
}

template<typename A, typename B>
struct is_same {
    static constexpr bool value = false;
};

template<typename T>
struct is_same<T, T> {
    static constexpr bool value = true;
};

template<typename T>
struct non_neg {
    static constexpr bool value =
            is_same<T, unsigned char>::value ||
            is_same<T, unsigned short>::value ||
            is_same<T, unsigned int>::value ||
            is_same<T, unsigned long>::value ||
            is_same<T, unsigned long long>::value;
};

template<typename T>
T neg(T x) {
    if constexpr (non_neg<T>::value) {
        throw std::runtime_error("neg: TypeError.");
    } else {
        return -x;
    }
}

int main() {
    const float rt = auto_sqrt(2.f);
    std::cout << rt << std::endl;
    constexpr int a = 1;
    std::cout << neg(a) << std::endl;
    constexpr unsigned b = 1;
    std::cout << neg(b) << std::endl;
}
