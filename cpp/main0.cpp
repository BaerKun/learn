#include <cstring>
#include <iostream>

/*
 * 元编程（Metaprogramming）
 * 简单来说，就是写代码来操作代码。
 * 在 C++ 中，它通常指的是在编译期执行的编程逻辑，
 * 比如类型推导、条件判断、循环等——这些不是运行时发生的，而是在编译器处理源代码时完成的。
 * 可以把元编程理解为：
 * 写“程序”去生成或优化另一个“程序”。
 */

// c++11: 引入constexpr(常量表达式)关键字
// 通常不产生运行时内存，除非被取地址
constexpr int ONE = 1;
// const int *p = &ONE;

constexpr int square(const int x) {
    return x * x;
}

// c++11：必须单return语句，只能调用constexpr函数，支持递归，不支持lambda
constexpr int power(const int base, const int exp) {
    return exp == 0 ? ONE : square(power(base, exp >> ONE)) * (exp & ONE ? base : ONE);
}

// constexpr函数也能作为普通函数
int power_(const int base, const int exp) {
    return exp == 0 ? 1 : square(power(base, exp >> 1)) * (exp & 1 ? base : 1);
}

// c++14: 支持多行，局部变量，循环，lambda
constexpr size_t strlen_constexpr(const char *str) {
    size_t size = 0;
    while (*str++ != '\0') ++size;
    return size;
}

// constexpr库函数：依赖编译器
constexpr size_t strlen_(const char *str) {
    return strlen(str);
}

int main() {
    constexpr int a = power(2, 10);
    const int b = power_(2, 11);
    std::cout << a << std::endl << b << std::endl;

    constexpr size_t len = strlen_constexpr("Hello World");
    std::cout << len << std::endl;
}
