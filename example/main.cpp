
#include <iostream>
#include <vector>
#include <cassert>

#include "process/Process.hpp"

int main() {
	process::Process p("./echoer");

	constexpr std::size_t data_len = 1000;

	std::vector<char> data(data_len, 'z');
	p.writeExact(data.data(), data.size());
	std::cout << "finished writing" << std::endl;

	p.closeStdin();
	std::cout << "closed socket" << std::endl;

	std::vector<char> recieved(data_len, 0);
	p.readExact(recieved.data(), recieved.size());
	std::cout << "finished reading" << std::endl;

	if (recieved == data) {
		std::cout << "Success" << std::endl;
	}
	else {
		std::cout << "Failed" << std::endl;
	}

	return 0;
}
