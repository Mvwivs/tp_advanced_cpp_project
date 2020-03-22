
#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include <sys/wait.h>

#include "pipe.hpp"
#include "exec_utils.hpp"

namespace process {

// Run executable in child process with stdin/stdout connected to parent process
class Process {
public:
	// Run with specified path to executable
	template <typename... Args>
	Process(const std::string& path, Args... args);
	Process(Process&& other);
	Process& operator=(Process&& other);
	Process() = delete;
	Process& operator=(const Process& other) = delete;
	Process(const Process& other) = delete;
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
	DuplexDescriptor descriptor_;	// Desciptor used to commuticate with child
	pid_t pid_;						// pid of child process
};

template <typename... Args>
Process::Process(const std::string& path, Args... args) :
	pid_(-1) {

	auto [parent, child] = createDuplexPipe();

	pid_ = fork();
	if (pid_ < 0) {
		throw std::runtime_error(std::string("Error, unable to fork: ") + std::strerror(errno));
	}
	else if (pid_ == 0) { // child
		try {
			close();
			parent.close();
			child.redirectToStd();
			exec(path, args...);
			// if exec failed
			throw std::runtime_error(std::string("Error, exec wasn't called: ") + std::strerror(errno));
		}
		catch (const std::exception& e) {
			child.close();
			_Exit(EXIT_FAILURE); // maybe print exception?
		}
	}
	else { // parent
		descriptor_ = std::move(parent);
	}
}


} // namespace process
