
#pragma once

#include <stdexcept>
#include <string>
#include <memory>

#include <sys/wait.h>

#include "pipe.hpp"

namespace process {

using namespace std::string_literals;

class Process {
public:
	explicit Process(const std::string& path);
	~Process();

	size_t write(const void* data, size_t len);
	void writeExact(const void* data, size_t len);
	size_t read(void* data, size_t len);
	void readExact(void* data, size_t len);

	bool isReadable() const;
	void closeStdin();

	void close();

	int join();
	void kill();

private:
	DuplexDescriptor descriptor;
	pid_t pid;
};

} // namespace process
