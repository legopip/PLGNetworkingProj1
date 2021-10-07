#include <iostream>

int main(int argc, char** argv) {
	printf("\x1B[31mLucas\033[0m\t\t");
	std::cout << "is nice" << std::endl;

	printf("\x1B[34mPhilip\033[0m\t\t");
	std::cout << "is also nice" << std::endl;

	printf("\x1B[32mGian\033[0m\t\t");
	std::cout << "is like the worst" << std::endl;

	

	return 1;
}