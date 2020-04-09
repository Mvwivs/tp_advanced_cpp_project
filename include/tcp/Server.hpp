
#pragma once

#include "connection_utils.hpp"
#include "Connection.hpp"

namespace tcp {

// TCP server, allow to listens and accept connections
class Server {
public:
	explicit Server(const Address& address, bool reuse_addr = false);

	// Open server on specified address
	void open(const Address& address, bool reuse_addr);

	Server() = delete;
	Server(const Server& other) = delete;
	Server& operator=(const Server& other) = delete;

	Server(Server&& other);

	Server& operator=(Server&& other);

	~Server();

	// Close server
	void close();

	// Wait for client, accept and return connection
	Connection accept() const;

	// Check if server is open
	bool is_open() const;

	// Set max connection count for listening
	void set_max_connect(int max_connections);

private:
	int fd_;	// Underlying listening server socket
	static const int default_max_connect = 100; // Defualt connection count for listening

};

}
