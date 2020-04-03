
#pragma once

#include <stdexcept>
#include <string>
#include <cstdint>

#include <arpa/inet.h>

namespace tcp {

using namespace std::string_literals;

class ConnectionException : public std::runtime_error {
public:
	ConnectionException(const std::string& messsage):
		std::runtime_error(messsage) {
	}
	~ConnectionException() = default;
	ConnectionException() = delete;
};

struct Address {
	std::uint32_t ip;
	std::uint16_t port;

	Address(std::uint32_t ip_addr, std::uint16_t port_addr) :
		ip(ip_addr),
		port(port_addr) {
	}
	Address(const std::string& ip_addr, std::uint16_t port_addr):
		port(port_addr) {
		in_addr in{};
		int res = inet_pton(AF_INET, ip_addr.c_str(), &in);
		if (res != 1) {
			throw ConnectionException("Unable to convert ip from string: "s + std::strerror(errno));
		}
		ip = ntohl(in.s_addr);
	}

	Address() = default;
	~Address() = default;

	std::string ip_as_string() const {
		in_addr in{};
		in.s_addr = htonl(ip);
		char str[INET_ADDRSTRLEN];
		const char* res = inet_ntop(AF_INET, &in, str, INET_ADDRSTRLEN);
		if (!res) {
			throw ConnectionException("Unable to convert ip to string: "s + std::strerror(errno));
		}
		return { str };
	}
};

Address get_source_address(int fd) {
	sockaddr_in src{};
	socklen_t len = sizeof(src);
	int res = getsockname(fd, reinterpret_cast<sockaddr*>(&src), &len);
	if (res == -1) {
		throw ConnectionException("Unable to get src address: "s + std::strerror(errno));	
	}
	return { ntohl(src.sin_addr.s_addr), ntohs(src.sin_port) };
}

Address get_destination_address(int fd) {
	sockaddr_in src{};
	socklen_t len = sizeof(src);
	int res = getpeername(fd, reinterpret_cast<sockaddr*>(&src), &len);
	if (res == -1) {
		throw ConnectionException("Unable to get src address: "s + std::strerror(errno));	
	}
	return { ntohl(src.sin_addr.s_addr), ntohs(src.sin_port) };
}

}
