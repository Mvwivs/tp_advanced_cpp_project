
#include <iostream>
#include <vector>
#include <cassert>

#include "process/Process.hpp"

int main() {
	Process p("./echoer");

	constexpr std::size_t data_len = 2048;

	std::vector<char> data(data_len, 'z');
	p.writeExact(data.data(), data.size());
	p.closeStdin();

	char buff[data_len] = {};
	p.readExact(buff, data.size());
	std::vector<char> recieved(buff, buff + data.size());
	if (recieved == data) {
		std::cout << "Success" << std::endl;
	}
	else {
		std::cout << "Failed" << std::endl;
	}

	return 0;
}
