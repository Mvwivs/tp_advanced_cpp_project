
#pragma once

#include "connection_utils.hpp"
#include "Connection.hpp"

namespace tcp {

class Server {
public:
	explicit Server(Address address, bool reuse_addr = false);

	void open(Address address, bool reuse_addr);

	Server() = delete;
	Server(const Server& other) = delete;
	Server& operator=(const Server& other) = delete;

	Server(Server&& other);

	Server& operator=(Server&& other);

	~Server();

	void close();

	Connection accept();

	bool is_open() const;

	void set_max_connect(int max_connections);

private:
	int fd_;
	static const int default_max_connect = 100;

};

}
