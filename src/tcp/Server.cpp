
#include "tcp/Server.hpp"

#include <cstring>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace tcp {

using namespace std::string_literals;

Server::Server(Address address, bool reuse_addr) :
	fd_(-1) {

	open(address, reuse_addr);
}

void Server::open(Address address, bool reuse_addr) {
	close();

	fd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd_ == -1) {
		close();
		throw ConnectionException("Unable to create socket: "s + std::strerror(errno));
	}

	if (reuse_addr) {
		int enable = 1;
		int res = setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
		if (res == -1) {
			close();
			throw ConnectionException("Unable to bind socket: "s + std::strerror(errno));
		}
	}

	sockaddr_in local_address{};
	local_address.sin_family = AF_INET;
	local_address.sin_addr.s_addr = htonl(address.ip);
	local_address.sin_port = htons(address.port);

	int res = bind(fd_, reinterpret_cast<sockaddr*>(&local_address), sizeof(local_address));
	if (res == -1) {
		close();
		throw ConnectionException("Unable to bind socket: "s + std::strerror(errno));
	}

	res = listen(fd_, default_max_connect);
	if (res == -1) {
		close();
		throw ConnectionException("Unable to mark socket as listening: "s + std::strerror(errno));
	}
}

Server::Server(Server&& other) :
	fd_(other.fd_) {
	other.fd_ = -1;
}

Server& Server::operator=(Server&& other) {
	close();
	fd_ = other.fd_;
	other.fd_ = -1;
	other.close();
	return *this;
}

Server::~Server() {
	close();
}

void Server::close() {
	if (is_open()) {
		::close(fd_);
		fd_ = -1;
	}
}

Connection Server::accept() {
	sockaddr_in client_addr;
	socklen_t addr_size = sizeof(client_addr);
	int socket = ::accept(fd_, reinterpret_cast<sockaddr*>(&client_addr), &addr_size);
	if (socket == -1) {
		throw ConnectionException("Unable to accept: "s + std::strerror(errno));
	}
	return Connection(socket);
}

bool Server::is_open() const {
	return fd_ != -1;
}

void Server::set_max_connect(int max_connections) {
	int res = listen(fd_, max_connections);
	if (res == -1) {
		close();
		throw ConnectionException("Unable to mark socket as listening: "s + std::strerror(errno));
	}
}

}
