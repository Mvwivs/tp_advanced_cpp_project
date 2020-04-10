
#pragma once

#include <unordered_map>
#include <functional>

#include "AsyncConnection.hpp"

namespace tcp {

// Callback for server
using Callback = std::function<void(AsyncConnection&)>;

// Epoll server with non-blocking read write
class AsyncServer {
public:
	AsyncServer() = delete;
	AsyncServer(const Address& address, Callback callback, const EventSet& events);

	AsyncServer(const AsyncServer& other) = delete;
	AsyncServer& operator=(const AsyncServer& other) = delete;

	AsyncServer(AsyncServer&& other);
	AsyncServer& operator=(AsyncServer&& other);

	~AsyncServer();
	
	// Close server 
	void close();

	// Run server 
	void run();

private:
	// Handle event on existing client
	void handleClient(int socket, uint32_t event);

	// Accept new client
	void acceptClient();

	// Add new client to epoll
	void addEpoll(int socket, const EventSet& events);

	// Open listening server on address
	void openServer(const Address& address);

private:
	int epoll_fd_;		// Epoll descriptor
	int server_fd_;		// Listening server desctiptor

	Callback callback_;	// User callback
	std::unordered_map<int, AsyncConnection> connections_;	// Existing connections
	EventSet events_;	// Events, specified by user
};

}
