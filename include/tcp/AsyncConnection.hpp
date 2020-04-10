
#pragma once

#include <cstring>
#include <vector>
#include <utility>

#include <unistd.h>

#include "connection_utils.hpp"
#include "connection_state.hpp"

namespace tcp {

using namespace std::string_literals;

class AsyncConnection {
public:
	explicit AsyncConnection(int& socket) :
		fd_(socket) {
		try {
			source_ = get_source_address(fd_);
			destination_ = get_destination_address(fd_);
		}
		catch (const ConnectionException& e) {
			close();
			throw ConnectionException("Unable to get source and destination: "s + std::strerror(errno));
		}
	}

	AsyncConnection(const AsyncConnection& other) = delete;
	AsyncConnection& operator=(const AsyncConnection& other) = delete;
	AsyncConnection& operator=(AsyncConnection&& other) = delete;
	
	AsyncConnection(AsyncConnection&& other): 
		fd_(std::exchange(other.fd_, -1)),
		source_(std::exchange(other.source_, {})),
		destination_(std::exchange(other.destination_, {})),
		buffer_(std::exchange(other.buffer_, {})),
		events_(std::exchange(other.events_, {})) {

	}

	~AsyncConnection() {
		close();
	}

	void close() {
		close_fd(fd_);
		source_ = {};
		destination_ = {};
		events_.add(Event::Closed);
	}

bool is_open() const {
	return fd_ != -1;
}

const Address& source() const {
	return source_;
}

const Address& destintation() const {
	return destination_;
}

std::size_t write(const void* data, std::size_t len) {
	ssize_t written = ::write(fd_, data, len);
	if (written == -1) {
		if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
			return 0;
		}
		throw ConnectionException("Unable to write: "s + std::strerror(errno));
	}
	if (written == 0) {
		events_.add(Event::Closed);
	}
	return written;
}

std::size_t read(void* data, std::size_t len) {
	ssize_t recieved = ::read(fd_, data, len);
	if (recieved == -1) {
		if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
			return 0;
		}
		throw ConnectionException("Unable to write: "s + std::strerror(errno));
	}
	if (recieved == 0) {
		events_.add(Event::Closed);
	}
	return recieved;
}

std::size_t readToBuffer(std::size_t len) {
	std::vector<char> readBuffer(len);
	std::size_t recieved = read(readBuffer.data(), len);
	if (recieved == -1) {
		if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
			return 0;
		}
		throw ConnectionException("Unable to write: "s + std::strerror(errno));
	}
	if (recieved == 0) {
		events_.add(Event::Closed);
	}
	else {
		buffer_.insert(buffer_.end(), readBuffer.cbegin(), readBuffer.cbegin() + recieved);
	}
	return recieved;
}

void clearBuffer() {
	buffer_.clear();
}

const std::vector<char>& buffer() const {
	return buffer_;
}

bool hasEvent(Event event) const {
	return events_.isIn(event);
}

void setEvents(const EventSet& event_set) {
	events_ = event_set;
}

private:
	int fd_;
	Address source_;
	Address destination_;

	std::vector<char> buffer_;
	EventSet events_;
};

}
