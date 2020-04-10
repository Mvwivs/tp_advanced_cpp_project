
#include "tcp/AsyncConnection.hpp"

#include <cstring>
#include <utility>

#include <unistd.h>

namespace tcp {

using namespace std::string_literals;

AsyncConnection::AsyncConnection(int& socket) :
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

AsyncConnection::AsyncConnection(AsyncConnection&& other): 
	fd_(std::exchange(other.fd_, -1)),
	source_(std::move(other.source_)),
	destination_(std::move(other.destination_)),
	buffer_(std::move(other.buffer_)),
	events_(std::move(other.events_)) {
}

AsyncConnection& AsyncConnection::operator=(AsyncConnection&& other) {
	close();
	fd_ = std::exchange(other.fd_, -1);
	source_ = std::move(other.source_);
	destination_ = std::move(other.destination_);
	buffer_ = std::move(other.buffer_);
	events_ = std::move(other.events_);
	return *this;
}

AsyncConnection::~AsyncConnection() {
	close();
}

void AsyncConnection::close() {
	close_fd(fd_);
	source_ = {};
	destination_ = {};
	events_.add(Event::Closed);
}

bool AsyncConnection::is_open() const {
	return fd_ != -1;
}

const Address& AsyncConnection::source() const {
	return source_;
}

const Address& AsyncConnection::destintation() const {
	return destination_;
}

std::size_t AsyncConnection::write(const void* data, std::size_t len) {
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

std::size_t AsyncConnection::read(void* data, std::size_t len) {
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

std::size_t AsyncConnection::readToBuffer(std::size_t len) {
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

void AsyncConnection::clearBuffer() {
	buffer_.clear();
}

const std::vector<char>& AsyncConnection::buffer() const {
	return buffer_;
}

bool AsyncConnection::hasEvent(Event event) const {
	return events_.isIn(event);
}

void AsyncConnection::setEvents(const EventSet& event_set) {
	events_ = event_set;
}

}
