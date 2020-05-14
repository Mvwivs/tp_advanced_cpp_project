
#pragma once

#include <thread>
#include <atomic>

#include "tcp/connection_utils.hpp"
#include "logger/Logger.hpp"
#include "http/HTTP.hpp"
#include "http/Worker.hpp"
#include "http/fd.hpp"

namespace http {

// HTTP Server
class Server {
public:
	Server(const tcp::Address& address, 
		std::size_t worker_count = std::thread::hardware_concurrency() - 1);
	virtual ~Server() = default;

	// start server loop
	void run();
	// stop server loop
	void stop();
	// User logic: build response for client request
	virtual http::HTTP::Response onRequest(const http::HTTP::Request& request) = 0;

private:
	// open server socket abd start listening
	void openServer(const tcp::Address& address);
	// accept incoming client
	void acceptClient();

public:
	std::unique_ptr<logger::BaseLogger> stdout_log;	// log to stdout
	std::unique_ptr<logger::BaseLogger> stderr_log;	// log to stderr
	std::unique_ptr<logger::BaseLogger> file_log;	// log to file

private:
	std::atomic_bool running;		// is server running
	fd_t server;					// server socket
	fd_t epoll;						// server epoll for client connecting events

	std::vector<Worker> workers;	// workers processing client requests
	std::size_t load_balancing;		// counter for round-robin load balancing
};

}
