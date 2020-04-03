
#include <future>
#include <string>
#include <vector>
#include <iostream>

#include "tcp/Server.hpp"

void echo(tcp::Connection& connection) {
	char byte;
	for (std::size_t recieved; recieved = connection.read(&byte, 1);) {
		connection.writeExact(&byte, 1);
	}
}

bool check_read_write(tcp::Connection& connection) {
	std::vector<char> data = {'H', 'e', 'l', 'l', 'o'};
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
		to_client = std::move(server.accept());
		std::cout << "Client reconnected on: "
			<< to_client.destintation().ip_as_string() << ':' << to_client.destintation().port << std::endl;

		echo(to_client);

		return result;
	});

	tcp::Connection client(server_address);
	std::this_thread::sleep_for(std::chrono::seconds(1));

	std::cout << "Client connected to server on local port: " << client.source().port << std::endl;

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
