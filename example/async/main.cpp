
#include <future>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include "tcp/AsyncServer.hpp"
#include "tcp/AsyncConnection.hpp"
#include "tcp/connection_state.hpp"
#include "tcp/Connection.hpp"

void echo(tcp::Connection& connection) {
	constexpr std::size_t buf_size = 400;
	char buf[buf_size];
	for (std::size_t recieved; (recieved = connection.read(&buf, buf_size));) {
		connection.writeExact(&buf, recieved);
	}
}

int main() {

	tcp::Address server_address("127.0.0.1", 88881);

	std::future<bool> server_future = std::async([&server_address] {
		std::vector<char> data(100000000);
		std::generate(data.begin(), data.end(), [n = 0] () mutable { return (n++) % 100; });
		std::size_t to_read = data.size();
		std::size_t to_write = data.size();

		// need to use map to serve several clients
		auto handler = [to_read, to_write, &data](tcp::AsyncConnection& connection) mutable {
			if (connection.hasEvent(tcp::Event::ReadRdy)) {
				std::size_t recieved = connection.readToBuffer(to_read);
				to_read -= recieved;
				std::cout << "Recieved " << recieved << " bytes" << std::endl;
				if (to_read == 0) {
					if (connection.buffer() == data) {
						std::cout << "Success" << std::endl;
					}
					else {
						std::cout << "Failed" << std::endl;
					}
				}
			}
			if (connection.hasEvent(tcp::Event::WriteRdy)) {
				if (to_write != 0) {
					std::size_t sent = connection.write(data.data() + (data.size() - to_write), to_write);
					std::cout << "Sent " << sent << " bytes" << std::endl;
					to_write -= sent;
				}
			}
		};
		tcp::AsyncServer server(server_address, handler, 
			{tcp::Event::WriteRdy, tcp::Event::ReadRdy, tcp::Event::Closed});
		server.run();

		return true;
	});

	std::this_thread::sleep_for(std::chrono::seconds(1));

	tcp::Connection client(server_address);
	echo(client);
	client.close();

	server_future.get();

	return 0;
}
