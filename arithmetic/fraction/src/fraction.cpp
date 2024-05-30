#include "../include/fraction.h"
#include "not_implemented.h"

fraction::fraction(
	big_int &&numerator,
	big_int &&denominator):
        _numerator(numerator),
        _denominator(denominator)
{
	simplify(*this);
}


fraction &fraction::operator+=(fraction const &other)
{
	big_int deletil = find_denominator(*this, other);

	big_int first_multiplyer = deletil / _denominator;
	big_int second_multiplyer = deletil / other._denominator;

	_denominator = deletil;

	_numerator *= first_multiplyer;
	_numerator += (other._numerator * second_multiplyer);

	simplify(*this);
	return *this;
}

fraction fraction::operator+(fraction const &other) const
{
    fraction temp = *this;
	temp += other;
	return temp;
}

fraction &fraction::operator-=(fraction const &other)
{
	big_int deletil = find_denominator(*this, other);

	big_int first_multiplyer = deletil / _denominator;
	big_int second_multiplyer = deletil / other._denominator;

	_denominator = deletil;
	_numerator *= first_multiplyer;
	_numerator -= (other._numerator * second_multiplyer);

	simplify(*this);
	return *this;
}

fraction fraction::operator-(fraction const &other) const
{
	fraction temp = *this;
	temp -= other;
	return temp;
}

fraction &fraction::operator*=(fraction const &other)
{
	_numerator *= other._numerator;
	_denominator *= other._denominator;
	simplify(*this);
	return *this;
}

fraction fraction::operator*(fraction const &other) const
{
	fraction temp = *this;
	temp *= other;
	return temp;
}

fraction &fraction::operator/=(fraction const &other)
{
	_numerator *= other._denominator;
	_denominator *= other._numerator;

	simplify(*this);
	return *this;
}

fraction fraction::operator/(fraction const &other) const
{
	fraction temp = *this;
	temp /= other;
	return temp;
}

bool fraction::operator==(fraction const &other) const
{
    return (other._denominator == _denominator && other._numerator == _numerator);
}

bool fraction::operator!=(fraction const &other) const
{
	return !(*this == other);
}

bool fraction::operator>=(fraction const &other) const
{
    return (*this > other) || (*this == other);
}

bool fraction::operator>(fraction const &other) const
{
	big_int first = ((_numerator * other._denominator));
	big_int second = ((other._numerator * _denominator));
	return first > second;
}

bool fraction::operator<=(fraction const &other) const
{
	return (*this < other) || (*this == other);
}

bool fraction::operator<(fraction const &other) const
{
	big_int first =_numerator * other._denominator;
	big_int second = other._numerator * _denominator;
	return first < second;
}

std::ostream &operator<<(
    std::ostream &stream,
    fraction const &obj)
{
    stream << obj._numerator << "/" << obj._denominator;
	return stream;
}

std::istream &operator>>(
    std::istream &stream,
    fraction &obj)
{
	std::string a, b;
	char slash;
	stream >> a;

	// Считываем разделитель '/'
	stream >> slash;
	if (slash != '/') {
		stream.setstate(std::ios::failbit); // Устанавливаем флаг ошибки
		return stream;
	}

	stream >> b;
	obj = fraction(big_int(a), big_int(b));
	return stream;
}
fraction fraction::sin(fraction const &epsilon) const
{
	fraction result = fraction(big_int("0"), big_int("1"));
	fraction term = result +  fraction(big_int("2"), big_int("1")) * epsilon;
	size_t iteration = 0;
	int i = 1;
	while (term.abs() > epsilon)
	{
		term = fraction(big_int(std::to_string(i)), 1_bi);
		term *= pow(2 * iteration + 1);
		term /= fraction(big_int::factorial(big_int(std::to_string(2 * iteration + 1))), 1_bi);
		result += term;
		++iteration;
		i *= -1;
	}
	return result;
}

fraction fraction::cos(fraction const &epsilon) const
{
	fraction result = fraction(big_int("0"), big_int("1"));
	fraction term = result +  fraction(big_int("2"), big_int("1")) * epsilon;
	size_t iteration = 0;
	int i = 1;
	while (term.abs() > epsilon)
	{
		term = fraction(big_int(std::to_string(i)), 1_bi);
		term *= pow(2 * iteration);
		term /= fraction(big_int::factorial(big_int(std::to_string(2 * iteration))), 1_bi);
		result += term;
		++iteration;
		i *= -1;
	}
	return result;
}

fraction fraction::tg(fraction const &epsilon) const
{
	fraction sin_value = this->sin(epsilon / fraction(big_int("2"), big_int("1")));
	fraction cos_value = this->cos(epsilon / fraction(big_int("2"), big_int("1")));
	fraction result = sin_value / cos_value;
	return result;
}

fraction fraction::ctg(
	fraction const &epsilon) const
{
	fraction sin_value = this->sin(epsilon / fraction(big_int("2"), big_int("1")));
	fraction cos_value = this->cos(epsilon / fraction(big_int("2"), big_int("1")));
	fraction result = cos_value / sin_value;
	return result;
}

fraction fraction::sec(
	fraction const &epsilon) const
{

	fraction cos_value = this->cos(epsilon / fraction(big_int("2"), big_int("1")));
	fraction result = fraction(big_int("1"), big_int("1"))/ cos_value;
	return result;
}

fraction fraction::cosec(
	fraction const &epsilon) const
{
	fraction sin_value = this->sin(epsilon / fraction(big_int("2"), big_int("1")));
	fraction result = fraction(big_int("1"), big_int("1"))/ sin_value;
	return result;
}

fraction fraction::arcsin(
    fraction const &epsilon) const
{
	fraction result =  *this;
	fraction term = fraction(1_bi, 1_bi);
	size_t iteration = 1;

	while (term.abs() > epsilon)
	{
		size_t precalc = 1;
		for (size_t i = 1; i < 2 * i - 1; i+=2)
		{
			precalc *= i;
		}

		term = fraction(2_bi, 1_bi).pow(iteration);
		term *= fraction(big_int::factorial(iteration), 1_bi);
		term *= fraction((2 * iteration + 1), 1_bi);
		term = fraction(1_bi, 1_bi) / term;

		term *= fraction(precalc, 1_bi);
		term *= pow(2 * iteration + 1);

		result += term;
		++iteration;
	}
	return result;
}

fraction fraction::arccos(
    fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::arccos(fraction const &) const", "your code should be here...");
}

fraction fraction::arctg(
    fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::arctg(fraction const &) const", "your code should be here...");
}

fraction fraction::arcctg(
    fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::arcctg(fraction const &) const", "your code should be here...");
}

fraction fraction::arcsec(
    fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::arcsec(fraction const &) const", "your code should be here...");
}

fraction fraction::arccosec(
    fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::arccosec(fraction const &) const", "your code should be here...");
}

fraction fraction::pow(size_t degree) const
{
	if (degree == 0)
	{
		return fraction(1_bi, 1_bi);
	}
    if (degree == 1)
	{
		return *this;
	}
	if (degree % 2 == 0)
	{
		fraction temp = *this;
		temp = temp.pow(degree / 2);
		temp *= temp;
		return temp;
	}
	else
	{
		fraction temp = *this;
		temp = temp.pow(degree-1);
		temp *= (*this);
		return temp;
	}
}

fraction fraction::root(
    size_t degree,
    fraction const &epsilon) const
{
	fraction x = (*this);
	bool swapped;
	if (x._numerator > x._denominator)
	{
		std::swap(x._numerator, x._denominator);
		swapped = true;
	}
	fraction alpha = fraction(1_bi, big_int(std::to_string(degree)));
	x -= fraction(big_int("1"), big_int("1"));

	fraction result = fraction(big_int("1"), big_int("1"));
	fraction term = fraction(big_int("2"), big_int("1")) * epsilon;
	size_t iteration = 1;

	while (term.abs() > epsilon)
	{
		fraction precalc = alpha;
		for (int i = 1; i < iteration; ++i)
		{
			precalc *= (alpha - fraction(big_int(std::to_string(i)), 1_bi));
		}

		term = precalc;
		term *= x.pow(iteration);
		term *= fraction(1_bi, big_int::factorial(iteration));
		//std::cout<<term << '\n';
		result += term;
		++iteration;
	}
	if (swapped)
	{
		std::swap(result._denominator, result._numerator);
	}
	return result;
}

fraction fraction::log2(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::log2(fraction const &) const", "your code should be here...");
}

fraction fraction::ln(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::ln(fraction const &) const", "your code should be here...");
}

fraction fraction::lg(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::lg(fraction const &) const", "your code should be here...");
}

void fraction::simplify(fraction &f)
{
	bool is_negative = false;
	if (f._denominator < 0_bi && f._numerator < 0_bi)
	{
		f._denominator *= big_int("-1");
		f._numerator *= big_int("-1");
	}
	else if (f._denominator < 0_bi )
	{
		f._denominator *= big_int("-1");
		is_negative = true;
	}
	else if (f._numerator < 0_bi )
	{
		f._numerator *= big_int("-1");
		is_negative = true;
	}

	big_int gcd = f._denominator, b = f._numerator;
	if (gcd < b) { std::swap(gcd, b); } // ex
	while (b != big_int(0))
	{
		big_int temp = b;
		b = gcd % b;
		gcd = temp;
	}
	f._numerator /= gcd;
	f._denominator /= gcd;

	if (is_negative)
	{
		f._numerator *= big_int("-1");
	}
	// big_int lcm = (f._denominator * f._numerator) / (gcd);

}

big_int fraction::find_denominator(const fraction& a, const fraction& sec)
{
	big_int gcd = a._denominator;
	big_int b = sec._denominator;
	if (gcd < b) { std::swap(gcd, b); }
	while (b != big_int(0))
	{
		big_int temp = b;
		b = gcd % b;
		gcd = temp;
	}
	big_int lcm = (a._denominator * sec._denominator) / (gcd);
	return lcm;
}
fraction fraction::abs() const
{
	fraction temp = *this;
	simplify(temp);

	if (temp._numerator < 0_bi)
	{
		temp._numerator *= big_int("-1");
	}
	return temp;
}

