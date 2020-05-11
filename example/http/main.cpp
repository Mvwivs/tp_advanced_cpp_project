
#include <future>
#include <string>
#include <vector>
#include <iostream>

#include "tcp/connection_utils.hpp"
#include "tcp/Connection.hpp"
#include "http/Server.hpp"

int main() {

	tcp::Address server_address("127.0.0.1", 8888);

	http::Server server(server_address);

	std::future<bool> server_future = std::async([&server] {
		server.run();
		return true;
	});

	tcp::Connection client(server_address);

	std::string data = "GET /api/v2 HTTP/1.1\r\n"
						"Host: www.nowhere123.com\r\n"
						"Accept: image/gif, image/jpeg, */*\r\n"
						"Accept-Language: en-us\r\n"
						"Accept-Encoding: gzip, deflate\r\n"
						"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n"
						"Content-Length: 3\r\n"
						"\r\n"
						"123";
	client.writeExact(data.data(), data.size());
	while (true) {
		char c;
		client.readExact(&c, 1);
		std::cout << c;
	}

	return 0;
}
