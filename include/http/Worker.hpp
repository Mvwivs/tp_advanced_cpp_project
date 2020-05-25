
#pragma once

#include <functional>
#include <atomic>
#include <unordered_map>

#include "http/HTTP.hpp"
#include "http/Fd.hpp"
#include "http/ClientState.hpp"


namespace http {

using FormResponse_cb = std::function<http::HTTP::Response(const http::HTTP::Request&)>;

// Worker handling user requests
class Worker {
public:

	Worker(FormResponse_cb callback);
	Worker(Worker&& other);

	Worker(const Worker& other) = delete;
	Worker& operator=(const Worker& other) = delete;

	// add connected 
	void addClient(int client_fd);
	// run worker
	void run();
	// stop worker
	void stop();

private:
	// handle client on data (start coroutine)
	bool handleClient(int fd);

	// serve client (in coroutine)
	void serveClient(int fd);

private:
	std::atomic_bool running;						// is working running
	std::unordered_map<int, ClientState> clients;	// currently connected clints and their states
	Fd epoll;										// epoll for read/write data

	FormResponse_cb formResponse;					// user callback forming response
};

}
