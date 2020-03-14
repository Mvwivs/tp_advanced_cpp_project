
#include <iostream>
#include <vector>
#include <cassert>

#include "process/Process.hpp"

int main() {
	Process p("./echoer");

	constexpr std::size_t data_len = 1000;

	std::vector<char> data(data_len, 'z');
	p.writeExact(data.data(), data.size());
	std::cerr << "finished writing" << std::endl;
	p.closeStdin();
	std::cerr << "finished closing" << std::endl;

	char buff[data_len] = {};
	p.readExact(buff, data.size());
	std::cerr << "finished reading" << std::endl;
	std::vector<char> recieved(buff, buff + data.size());
	if (recieved == data) {
		std::cout << "Success" << std::endl;
	}
	else {
		std::cout << "Failed" << std::endl;
	}

	return 0;
}
