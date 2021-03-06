
#include <future>
#include <string>
#include <vector>
#include <iostream>
#include <csignal>

#include "tcp/connection_utils.hpp"
#include "tcp/Connection.hpp"
#include "http/Server.hpp"

static std::function<void(int)> signalHandler = [] (int) {
};
static void sig_handler(int signal) {
	signalHandler(signal);
}

namespace HTTP = http::HTTP;

class MyServer : public http::Server {
public:
	MyServer(const tcp::Address& address) :
		http::Server(address) {
	}
	~MyServer() = default;
	HTTP::Response onRequest(const HTTP::Request& request) override {
		http::Server::stdout_log->info("Recieved request to " + request.startLine.target);
		if (request.parameters) {
			http::Server::stdout_log->debug("Parameters: ");
			for (const auto& [name, value] : *request.parameters) {
				http::Server::stdout_log->debug("\t" + name + " -> " + value);
			}
		}
		http::Server::stdout_log->debug("Headers: ");
		for (const auto& [name, value] : request.headers) {
			http::Server::stdout_log->debug("\t" + name + " -> " + value);
		}
		
		HTTP::Response resp {
			{HTTP::Version::HTTP_1_1, http::HTTP::StatusCode{200}},
			{{"Connection", "Keep-Alive"}},
			{"<some> HTML </some>"}
		};
		return resp;
	}
};

int main() {
	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);

	tcp::Address server_address("127.0.0.1", 8888);

	MyServer server(server_address);
	signalHandler = [&server] (int) { server.stop(); };
	std::thread w([&server] { server.run(); });

	tcp::Connection client(server_address);
	std::string data = "GET /api/v2?parameter=value&also=another HTTP/1.1\r\n"
						"Host: www.nowhere123.com\r\n"
						"Accept: image/gif, image/jpeg, */*\r\n"
						"Accept-Language: en-us\r\n"
						"Accept-Encoding: gzip, deflate\r\n"
						"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n"
						"Content-Length: 3\r\n"
						"\r\n"
						"123";
	client.writeExact(data.data(), data.size());

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	std::cout << "Client recieved test response:" << std::endl;
	char c[100];
	for (std::size_t received = 0; (received = client.read(&c, 100));) {
		std::cout.write(c, received);
	}
	std::cout << std::endl;

	std::this_thread::sleep_for(std::chrono::seconds(20));
	server.stop();

	w.join();

	return 0;
}
