
#pragma once

#include <vector>

#include "connection_utils.hpp"
#include "connection_state.hpp"

namespace tcp {

class AsyncConnection {
public:
	explicit AsyncConnection(int& socket);

	AsyncConnection(const AsyncConnection& other) = delete;
	AsyncConnection& operator=(const AsyncConnection& other) = delete;
	AsyncConnection& operator=(AsyncConnection&& other) = delete;

	AsyncConnection(AsyncConnection&& other);

	~AsyncConnection();

	void close();

	bool is_open() const;

	const Address& source() const;

	const Address& destintation() const;

	std::size_t write(const void* data, std::size_t len);

	std::size_t read(void* data, std::size_t len);

	std::size_t readToBuffer(std::size_t len);

	void clearBuffer();

	const std::vector<char>& buffer() const;

	bool hasEvent(Event event) const;

	void setEvents(const EventSet& event_set);

private:
	int fd_;
	Address source_;
	Address destination_;

	std::vector<char> buffer_;
	EventSet events_;
};

}
