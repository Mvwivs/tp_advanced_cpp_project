
#pragma once

#include <cstdint>

#include "connection_utils.hpp"

namespace tcp {

class Server;

class Connection {
public:
	explicit Connection(Address address);
	friend class Server;

	void connect(Address address);

	std::size_t write(const void* data, std::size_t len);

	std::size_t read(void* data, std::size_t len);

	void writeExact(const void* data, std::size_t len);

	void readExact(void* data, std::size_t len);

	Connection() = delete;

	Connection(const Connection& other) = delete;
	Connection& operator=(const Connection& other) = delete;

	Connection(Connection&& other);

	Connection& operator=(Connection&& other);

	~Connection();

	void close();

	bool is_open() const;

	void set_timeout(int timeout_s);

	const Address& source() const;

	const Address& destintation() const;

private:
	explicit Connection(int& socket);

private:
	int fd_;
	Address source_;
	Address destintation_;
};

}
