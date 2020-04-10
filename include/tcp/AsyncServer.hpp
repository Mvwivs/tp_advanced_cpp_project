
#pragma once

#include <unordered_map>
#include <functional>

#include "AsyncConnection.hpp"

namespace tcp {

using Callback = std::function<void(AsyncConnection&)>;

class AsyncServer {
public:
	AsyncServer() = delete;
	AsyncServer(const Address& address, Callback callback, const EventSet& events);

	AsyncServer(const AsyncServer& other) = delete;
	AsyncServer operator=(const AsyncServer& other) = delete;

	~AsyncServer();
	void close();

	void run();

private:
	void handleClient(int socket, uint32_t event);

	void acceptClient();

	void addEpoll(int socket, const EventSet& events);

	void openServer(const Address& address);

private:
	int epoll_fd_;
	int server_fd_;

	Callback callback_;
	std::unordered_map<int, AsyncConnection> connections_;
	EventSet events_;
};
}
