
#pragma once

#include <string>
#include <cstdint>
#include <cstring>
#include <stdexcept>

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "connection_utils.hpp"

namespace tcp {

class Server;

class Connection {
public:
	explicit Connection(Address address) :
		fd_(-1) {
		connect(address);
	}

	friend class Server;

	void connect(Address address) {
		close(); // close previous connection

		fd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (fd_ == -1) {
			close();
			throw ConnectionException("Unable to create socket: "s + std::strerror(errno));
		}

		// TODO: add timeout

		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(address.ip);
		addr.sin_port = htons(address.port);

		int res = ::connect(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
		if (res == -1) {
			close();
			throw ConnectionException("Unable to connect: "s + std::strerror(errno));	
		}

		source_ = get_source_address(fd_);
		destintation_ = address;
	}

	std::size_t write(const void* data, std::size_t len) {
		ssize_t written = ::write(fd_, data, len);
		if (written == -1) {
			throw ConnectionException("Unable to write: "s + std::strerror(errno));
		}
		return written;
	}

	std::size_t read(void* data, std::size_t len) {
		ssize_t recieved = ::read(fd_, data, len);
		if (recieved == -1) {
			throw ConnectionException("Unable to read: "s + std::strerror(errno));
		}
		return recieved;
	}

	void writeExact(const void* data, std::size_t len) {
		std::size_t written = 0;
		const char* d = static_cast<const char*>(data);
		while (len - written != 0) {
			std::size_t res = write(d + written, len - written);;
			if (res == 0) {
				throw ConnectionException("Error, unable to writeExact, EOF: "s + std::strerror(errno));
			}
			written += res;
		}
	}

	void readExact(void* data, std::size_t len) {
		std::size_t recieved = 0;
		char* d = static_cast<char*>(data);
		while (len - recieved != 0) {
			std::size_t res = read(d + recieved, len - recieved);
			if (res == 0) {
				throw ConnectionException("Error, unable to readExact: EOF");
			}
			recieved += res;
		}
	}

	Connection() = delete;

	Connection(const Connection& other) = delete;
	Connection& operator=(const Connection& other) = delete;

	Connection(Connection&& other) :
		fd_(other.fd_),
		source_(other.source_),
		destintation_(other.destintation_) {
		other.fd_ = -1;
		other.close();
	}

	Connection& operator=(Connection&& other) {
		close();
		fd_ = other.fd_;
		source_ = other.source_;
		destintation_ = other.destintation_;
		other.fd_ = -1;
		other.close();
		return *this;
	}

	~Connection() {
		close();
	}

	void close() {
		if (is_open()) {
			::close(fd_);
			fd_ = -1;
		}
		source_ = {};
		destintation_ = {};
	}

	bool is_open() const {
		return fd_ != -1;
	}

	void set_timeout(int timeout_s) {
		timeval timeout{};
		timeout.tv_sec = timeout_s;
		int res = setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO | SO_RCVTIMEO, &timeout, sizeof(timeout));
		if (res == -1) {
			throw ConnectionException("Unable to create socket: "s + std::strerror(errno));
		}
	}

	const Address& source() const {
		return source_;
	}

	const Address& destintation() const {
		return destintation_;
	}

private:
	explicit Connection(int& socket) :
		fd_(socket) {
		try {
			source_ = get_source_address(fd_);
			destintation_ = get_destination_address(fd_);
		}
		catch (const ConnectionException& e) {
			close();
			throw ConnectionException("Unable to get source and destination: "s + std::strerror(errno));
		}
	}

private:
	int fd_;
	Address source_;
	Address destintation_;
};

}
