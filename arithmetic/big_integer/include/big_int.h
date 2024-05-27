#ifndef MP_OS_BIG_INT_H
#define MP_OS_BIG_INT_H

#include <vector>
#include <utility>
#include <iostream>
#include <compare>

class big_int
{
    // Call optimise after every operation!!!
    bool sign; // 1 +  0 -
    std::vector<unsigned int> digits;

    static void plus_assign_no_sign(std::vector<unsigned int>& lhs, const std::vector<unsigned int>& rhs, size_t shift = 0);

    static void minus_assign_no_sign(std::vector<unsigned int>& lhs, const std::vector<unsigned int>& rhs, size_t shift = 0);

    static void minus_assign_no_sign_reverse(const std::vector<unsigned int>& lhs, std::vector<unsigned int>& rhs, size_t shift = 0);

    static void multiply_assign_no_sign(std::vector<unsigned int>& lhs, const std::vector<unsigned int>& rhs);

    static std::vector<unsigned int> multiply_common(const std::vector<unsigned int>& lhs, const std::vector<unsigned int>& rhs);

    static std::vector<unsigned int> multiply_karatsuba(const std::vector<unsigned int>& lhs, const std::vector<unsigned int>& rhs);

    static void divide_assign_no_sign(std::vector<unsigned int>& lhs, const std::vector<unsigned int>& rhs);

    static std::vector<unsigned int> divide_common(const std::vector<unsigned int>& lhs, const std::vector<unsigned int>& rhs);

    static std::vector<unsigned int> divide_newton(const std::vector<unsigned int>& lhs, const std::vector<unsigned int>& rhs);

    void optimise() noexcept; // erases leading zeros, if number equals 0 sets sign to true

    static std::strong_ordering compare_no_sign(const std::vector<unsigned int>& lhs, const std::vector<unsigned int>& rhs) noexcept;
public:
    std::strong_ordering compare(const big_int& other, size_t shift = 0) const noexcept; // shifts other right by digits(2^(8*sizeof(unsigned int)))

public:

    explicit big_int(std::vector<unsigned int> const &digits, bool sign = true);
    big_int(int d);
    big_int();

    operator bool(); //false if 0 , else true

    big_int& operator++()&;
    big_int&& operator++()&&;
    big_int operator++(int);

    big_int& operator--()&;
    big_int&& operator--()&&;
    big_int operator--(int);

    big_int& operator+=(const big_int& other)&;
    big_int&& operator+=(const big_int&& other)&&;

    big_int& plus_assign(const big_int& other, size_t shift = 0)&;
    big_int&& plus_assign(const big_int&& other, size_t shift = 0)&&;


    big_int& operator-=(const big_int& other)&;
    big_int&& operator-=(const big_int&& other)&&;

    big_int& minus_assign(const big_int& other, size_t shift = 0)&;
    big_int&& minus_assign(const big_int&& other, size_t shift = 0)&&;

    big_int& operator*=(const big_int& other)&;
    big_int&& operator*=(const big_int&& other)&&;

    big_int& operator/=(const big_int& other)&;
    big_int&& operator/=(const big_int&& other)&&;

    big_int& operator%=(const big_int& other)&;
    big_int&& operator%=(const big_int&& other)&&;

    big_int operator+(const big_int& other) const;
    big_int operator-(const big_int& other) const;
    big_int operator*(const big_int& other) const;
    big_int operator/(const big_int& other) const;
    big_int operator%(const big_int& other) const;

    std::strong_ordering operator<=>(const big_int& other) const noexcept;

    big_int& operator<<=(size_t shift)&;
    big_int&& operator<<=(size_t shift)&&;

    big_int& operator>>=(size_t shift)&;
    big_int&& operator>>=(size_t shift)&&;


    big_int operator<<(size_t shift);
    big_int operator>>(size_t shift);

    big_int operator~() const;

    big_int& operator&=(const big_int& other)&;
    big_int&& operator&=(const big_int& other)&&;

    big_int& operator|=(const big_int& other)&;
    big_int&& operator|=(const big_int& other)&&;

    big_int& operator^=(const big_int& other)&;
    big_int&& operator^=(const big_int& other)&&;


    big_int operator&(const big_int& other);
    big_int operator|(const big_int& other);
    big_int operator^(const big_int& other);

    friend std::ostream &operator<<(std::ostream &stream, big_int const &value) noexcept;

    friend std::istream &operator>>(std::istream &stream, big_int &value);

    std::string to_string() const;
};


#endif //MP_OS_BIG_INT_H