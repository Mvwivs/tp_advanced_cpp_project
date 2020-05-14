
#pragma once

#include "logger/Logger.hpp"
#include "Worker.hpp"

namespace http {

class Server {
public:
	Server(const tcp::Address& address, 
		std::size_t worker_count = std::thread::hardware_concurrency() - 1);
	virtual ~Server() = default;

	void run();

	void stop();

	virtual http::HTTP::Response onRequest(const http::HTTP::Request& request) = 0;

private:
	void openServer(const tcp::Address& address);

	void acceptClient();

public:
	std::unique_ptr<logger::BaseLogger> stdout_log;
	std::unique_ptr<logger::BaseLogger> stderr_log;
	std::unique_ptr<logger::BaseLogger> file_log;

private:
	std::atomic_bool running;
	fd_t server;
	fd_t epoll;

	std::vector<Worker> workers;
	std::size_t load_balancing;
};

}
