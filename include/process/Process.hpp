
#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <sys/types.h>

#include "pipe.hpp"

namespace process {

// Run executable in child process with stdin/stdout connected to parent process
class Process {
public:
	// Run with specified path to executable
	explicit Process(const std::string& path, const std::vector<std::string>& args = {});
	Process(Process&& other);
	Process& operator=(Process&& other);
	Process() = delete;
	Process& operator=(const Process& other) = delete;
	Process(const Process& other) = delete;
	~Process();

	// Write data to child process, return written bytes count
	std::size_t write(const void* data, std::size_t len);

	// Write exactly len bytes of data to child process
	void writeExact(const void* data, std::size_t len);

	// Read data from child process, return read bytes count
	std::size_t read(void* data, std::size_t len);

	// Read exactly len bytes of data from child process
	void readExact(void* data, std::size_t len);

	// Check if read operation is available
	bool isReadable() const;

	// Close write descriptor
	void closeStdin();

	// Close all descriptors
	void close();

	// Wait for child process to finish, returns status
	int join();

	// Kill child process
	void kill();

private:
	DuplexDescriptor descriptor_;	// Desciptor used to commuticate with child
	pid_t pid_;						// pid of child process
};

} // namespace process
