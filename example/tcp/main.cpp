
#include <future>
#include <string>
#include <vector>
#include <iostream>

#include "tcp/connection_utils.hpp"
#include "tcp/Connection.hpp"
#include "tcp/Server.hpp"

void echo(tcp::Connection& connection) {
	constexpr std::size_t buf_size = 400;
	char buf[buf_size];
	for (std::size_t recieved; recieved = connection.read(&buf, buf_size);) {
		connection.writeExact(&buf, recieved);
	}
}

bool check_read_write(tcp::Connection& connection) {
	std::vector<char> data(10000, 'a');
	connection.writeExact(data.data(), data.size());

	std::vector<char> recieved(data.size());
	connection.readExact(recieved.data(), recieved.size());
	return data == recieved;
}

int main() {

	tcp::Address server_address("127.0.0.1", 8888);

	tcp::Server server(server_address, true);

	std::future<bool> server_future = std::async([&server] {

		tcp::Connection to_client = server.accept();
		std::cout << "Server recieved connection from: "
			<< to_client.destintation().ip_as_string() << ':' << to_client.destintation().port << std::endl;

		bool result = check_read_write(to_client);

		to_client.close();
		to_client = server.accept();
		std::cout << "Client reconnected on: "
			<< to_client.destintation().ip_as_string() << ':' << to_client.destintation().port << std::endl;

		echo(to_client);

		return result;
	});

	tcp::Connection client(server_address);

	echo(client);
	client.connect(server_address);

	bool client_result = check_read_write(client);
	client.close();
	bool server_result = server_future.get();

	if (client_result) {
		std::cout << "Client send/recv succeeded" << std::endl;
	}
	else {
		std::cout << "Client send/recv failed" << std::endl;
	}
	if (server_result) {
		std::cout << "Server send/recv succeeded" << std::endl;
	}
	else {
		std::cout << "Server send/recv failed" << std::endl;
	}

	return 0;
}
