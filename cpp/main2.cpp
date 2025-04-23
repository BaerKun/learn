#include <iostream>


template<int N>
int factorial() {
    return N * factorial<N - 1>();
}

template<>
int factorial<0>() {
    return 1;
}

// 推荐
template<int N>
struct Factorial {
    static const int value = N * Factorial<N - 1>::value;
};

template<>
struct Factorial<0> {
    static const int value = 1;
};

int main() {
    const int a = factorial<5>();
    const int b = Factorial<5>::value;
    std::cout << a << std::endl << b << std::endl;
}