#include <fraction.h>
#include <iostream>


int main(int argc,char *argv[])
{
	fraction eps (1_bi, 1000_bi);

	fraction a(big_int("7"), big_int("2"));
	fraction b(big_int("3"), big_int("5"));

	a = a.cos(eps);
	a  /= b;

	std::cout << a << std::endl;
	std::cout << "\n------------------------------------------\n";

	fraction f (big_int("27"), big_int("1"));

	std::cout << f.root(2, eps) << std::endl;

}