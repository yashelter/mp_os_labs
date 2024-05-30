#include <fraction.h>
#include <iostream>


int main(int argc,char *argv[])
{
	fraction a(big_int("7"), big_int("2"));
	fraction b(big_int("3"), big_int("5"));


	a = a.cos(fraction(1_bi, 10000_bi));
	std::cout << a._numerator  << '/'  << a._denominator << std::endl;
	a  /= b;
	std::cout << a._numerator  << '/'  << a._denominator << std::endl;
	//a.sin(fraction(1, 10));
	//std::cout << a._numerator  << '/'  << a._denominator << std::endl;


	std::cout << a << std::endl;
	std::cout << b << std::endl;

	a = a.cos(fraction(1_bi, 10000000000_bi));

	std::stringstream ss;
	fraction result_of_sum = (a + b) * fraction(10000000000_bi, 1_bi);

	std::cout << result_of_sum << std::endl;

}