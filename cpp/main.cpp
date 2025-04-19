#include <iostream>
#include <string>

constexpr inline uint64_t square(const uint64_t x){
    return x * x;
}

constexpr uint64_t power(const uint64_t base, const uint64_t exp){
    return exp == 0 ? 1 : square(power(base, exp >> 1)) * (exp & 1 ? base : 1);
}

constexpr uint64_t str_hash8(const char* str){
    uint64_t hash = 0;
    for(; *str != '\0'; ++str){
        hash <<= 8;
        hash = hash | *str;
    }
    return hash;
}

uint64_t power_(const uint64_t base, const uint64_t exp){
    return exp == 0 ? 1 : square(power(base, exp >> 1)) * (exp & 1 ? base : 1);
}

int main(){
//    constexpr char str[] = "Hello";
//    uint64_t hash = str_hash8(str);
    const uint64_t a = power(2, 61);
    const uint64_t b = power_(2, 63);
    std::cout << a + b << std::endl;
}