
#pragma once

#include <stdexcept>
#include <string>
#include <cstdint>

namespace tcp {

// Exception with network connections in runtime
class ConnectionException : public std::runtime_error {
public:
	ConnectionException(const std::string& messsage);
	ConnectionException() = delete;
};

// Network transport layer address
struct Address {
	std::uint32_t ip;	// IPv4 32-bit address
	std::uint16_t port;	// TCP/UDP 16-bit port

	Address(std::uint32_t ip_addr, std::uint16_t port_addr);
	Address(const std::string& ip_addr, std::uint16_t port_addr);

	Address() = default;
	~Address() = default;

	// Convert ip to string dot-number format
	std::string ip_as_string() const;
};

// Extract source ip and port from socket
Address get_source_address(int fd);

// Extract destination ip and port from socket
Address get_destination_address(int fd);

}
