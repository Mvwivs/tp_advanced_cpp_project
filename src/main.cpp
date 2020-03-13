
#include <iostream>
#include <string>
#include <cassert>

#include "process/Process.hpp"

int main() {
	Process p("./echoer");

	std::string data = "Test data to send";
	auto written = p.write(data.data(), data.size());
	p.closeStdin();
	std::cout << "Written: " << written << std::endl;

	char buff[1024] = {};
	auto read = p.read(buff, data.size());
	std::cout << "Read: " << read << std::endl;
	std::string recieved(buff);
	if (recieved == data) {
		std::cout << "Success" << std::endl;
	}
	else {
		std::cout << data << " != " << recieved << std::endl;
	}

	return 0;
}
