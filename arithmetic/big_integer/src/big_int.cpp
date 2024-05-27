#include "../include/big_int.h"

std::strong_ordering big_int::operator<=>(const big_int& other) const noexcept
{
    return compare(other, 0);
}

std::strong_ordering big_int::compare(const big_int &other, size_t shift) const noexcept
{
    if (sign != other.sign)
    {
        if (sign)
        {
            return std::strong_ordering::greater;
        }
        return std::strong_ordering::less;
    }
    if (other.digits.size() + shift != digits.size())
    {
        if  (sign && digits.size() > other.digits.size() + shift)
        {
            return std::strong_ordering::greater;
        }
        else if (sign)
        {
            return std::strong_ordering::less;
        }
        else if (digits.size() > other.digits.size() + shift)
        {
            return std::strong_ordering::less;
        }
        else
        {
            return std::strong_ordering::greater;
        }
    }
    for (int iteration = 0; iteration < digits.size(); ++iteration)
    {
        if (digits.size() - 1 - iteration <= 0)
        {
            if (digits[digits.size() - iteration - 1] == 0)
            {
                continue;
            }
            if (sign)
            {
                return std::strong_ordering::greater;
            }
            return std::strong_ordering::less;
        }

        for (int j = 0; j < sizeof(unsigned int); ++j)
        {
            if (((digits[digits.size() - iteration - 1] >> j) & 1)!= ((other.digits[other.digits.size() - iteration - 1] >> j) & 1))
            {
                bool is_less = (((digits[digits.size() - iteration - 1] >> j) & 1) < ((other.digits[other.digits.size() - iteration - 1] >> j) & 1));

                if (sign && is_less)
                {
                    return std::strong_ordering::less;
                }
                else if (sign)
                {
                    return std::strong_ordering::greater;
                }
                else if (is_less)
                {
                    return std::strong_ordering::greater;
                }
                return std::strong_ordering::less;

            }
        }

    }
    return std::strong_ordering::equivalent;
}

big_int::big_int()
{
    sign = true;
	digits = {};
}
big_int::big_int(std::vector<unsigned int> const &_digits, bool _sign) : digits(_digits), sign(_sign)
{

}