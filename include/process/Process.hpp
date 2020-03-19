
#pragma once

#include <stdexcept>
#include <string>

#include <sys/wait.h>

#include "pipe.hpp"

namespace process {

using namespace std::string_literals; // 's' literal for exceptions

// Run executable in child process with stdin/stdout connected to parent process
class Process {
public:
	// Run with specified path to executable
	explicit Process(const std::string& path);

	~Process();

	// Write data to child process, return written bytes count
	size_t write(const void* data, size_t len);

	// Write exactly len bytes of data to child process
	void writeExact(const void* data, size_t len);

	// Read data from child process, return read bytes count
	size_t read(void* data, size_t len);

	// Read exactly len bytes of data from child process
	void readExact(void* data, size_t len);

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
	DuplexDescriptor descriptor;	// Desciptor used to commuticate with child
	pid_t pid;						// pid of child process
};

} // namespace process
