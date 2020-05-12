
#include <future>
#include <string>
#include <vector>
#include <iostream>

#include "tcp/connection_utils.hpp"
#include "tcp/Connection.hpp"
#include "http/Server.hpp"

namespace HTTP = http::HTTP;

class MyServer : public http::Server {
public:
	MyServer(const tcp::Address& address) :
		http::Server(address) {
	}
	~MyServer() = default;
	HTTP::Response onRequest(const HTTP::Request& request) override {
		HTTP::Response resp {
			{HTTP::Version::HTTP_1_1, http::HTTP::StatusCode{200}},
			{{"Connection", "Keep-Alive"}},
			{"<some> HTML </some>"}
		};
		return resp;
	}
};

int main() {

	tcp::Address server_address("127.0.0.1", 8888);

	MyServer server(server_address);

	server.run();

	// tcp::Connection client(server_address);

	std::string data = "GET /api/v2 HTTP/1.1\r\n"
						"Host: www.nowhere123.com\r\n"
						"Accept: image/gif, image/jpeg, */*\r\n"
						"Accept-Language: en-us\r\n"
						"Accept-Encoding: gzip, deflate\r\n"
						"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n"
						"Content-Length: 3\r\n"
						"\r\n"
						"123";
	// client.writeExact(data.data(), data.size());
	// while (true) {
	// 	char c;
	// 	client.readExact(&c, 1);
	// 	std::cout << c;
	// }

	return 0;
}
