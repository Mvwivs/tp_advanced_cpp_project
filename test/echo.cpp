
#include <iostream>

int main(int argc, char *argv[]) {

	std::cerr << "Arguments: ";
	for (int i = 0; i < argc; ++i) {
		std::cerr << argv[i] << ", "; 
	}
	std::cerr << std::endl;

	while (std::cin) {
		char byte;
		std::cin >> byte;
		std::cout << byte;
	}

	return 0;
}
