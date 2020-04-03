
#pragma once

#include <stdexcept>
#include <string>
#include <cstdint>

namespace tcp {

class ConnectionException : public std::runtime_error {
public:
	ConnectionException(const std::string& messsage);
	~ConnectionException() = default;
	ConnectionException() = delete;
};

struct Address {
	std::uint32_t ip;
	std::uint16_t port;

	Address(std::uint32_t ip_addr, std::uint16_t port_addr);
	Address(const std::string& ip_addr, std::uint16_t port_addr);

	Address() = default;
	~Address() = default;

	std::string ip_as_string() const;
};

Address get_source_address(int fd);

Address get_destination_address(int fd);

}
