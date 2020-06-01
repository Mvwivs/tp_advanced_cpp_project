
#include <future>
#include <string>
#include <vector>
#include <iostream>
#include <csignal>

#include "tcp/connection_utils.hpp"
#include "tcp/Connection.hpp"
#include "file/DbServer.hpp"

int main() {
	tcp::Address server_address("127.0.0.1", 8888);

	file::DbServer server(server_address, "test.db");

	std::thread w([&server] { server.run(); });

	tcp::Connection client(server_address);
	std::string data = "GET /?key=2564 HTTP/1.1\r\n"
						"\r\n";
	client.writeExact(data.data(), data.size());

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	std::cout << "Client recieved test response:" << std::endl;
	char c[100];
	for (std::size_t received = 0; (received = client.read(&c, 100));) {
		std::cout.write(c, received);
	}
	std::cout << std::endl;


	w.join();

	return 0;
}
