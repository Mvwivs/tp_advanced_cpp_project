
#pragma once

#include <cstdint>

#include "connection_utils.hpp"

namespace tcp {

class Server; // forward declaration

// Represets tcp connection between two network nodes
class Connection {
public:
	explicit Connection(const Address& address);
	friend class Server;	// allow private constructor use

	// Connect to address (and close current connection)
	void connect(const Address& address);

	// Write len or less bytes of data
	std::size_t write(const void* data, std::size_t len) const;

	// Read len or less bytes of data
	std::size_t read(void* data, std::size_t len) const;

	// Try to write exactly len bytes of data
	void writeExact(const void* data, std::size_t len) const;

	// Try to read exactly len bytes of data
	void readExact(void* data, std::size_t len) const;

	Connection() = delete;

	Connection(const Connection& other) = delete;
	Connection& operator=(const Connection& other) = delete;

	Connection(Connection&& other);

	Connection& operator=(Connection&& other);

	~Connection();

	// Close current connection
	void close();

	// Check if connetion is open
	bool is_open() const;

	void set_timeout(int timeout_s) const;

	// Get address of source node
	const Address& source() const;

	// Get address of destination node
	const Address& destintation() const;

private:
	explicit Connection(int& socket);	// don't allow socket construction, except for friend classes

private:
	int fd_;				// Underlying socket
	Address source_;		// Address of source node
	Address destintation_;	// Address of destination node
};

}
