
#pragma once

#include <vector>

#include "connection_utils.hpp"
#include "connection_state.hpp"

namespace tcp {

// Connection with non-blocking operations
class AsyncConnection {
public:
	explicit AsyncConnection(int& socket);

	AsyncConnection(const AsyncConnection& other) = delete;
	AsyncConnection& operator=(const AsyncConnection& other) = delete;

	AsyncConnection(AsyncConnection&& other);
	AsyncConnection& operator=(AsyncConnection&& other);

	~AsyncConnection();

	// Close connection
	void close();

	// Check if connection is open
	bool is_open() const;

	// Get source address
	const Address& source() const;

	// Get destintation address
	const Address& destintation() const;

	// Write len or less bytes of data
	std::size_t write(const void* data, std::size_t len);

	// Read len or less bytes of data
	std::size_t read(void* data, std::size_t len);

	// Read len or less bytes into internal buffer
	std::size_t readToBuffer(std::size_t len);

	// Clear internal buffer
	void clearBuffer();

	// Get internal buffer
	const std::vector<char>& buffer() const;

	// Check if event happened in connection
	bool hasEvent(Event event) const;

	// Set events of the connection
	void setEvents(const EventSet& event_set);

private:
	int fd_;					// Connection socket
	Address source_;			// Source address of the connection
	Address destination_;		// Destination address of the connection

	std::vector<char> buffer_;	// Buffer for data read
	EventSet events_;			// Events specifying available connection operations and state
};

}
