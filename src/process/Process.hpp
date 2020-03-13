
#pragma once

#include <stdexcept>
#include <string>
#include <memory>

#include "Pipe.hpp"

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
private:
	DoubleDescriptor descriptor;
};

Process::Process(const std::string& path) {
	auto [parent, child] = createPipe();

	pid_t pid = fork();
	if (pid < 0) {
		throw std::runtime_error("Error, unable to fork: "s + std::strerror(errno));
	}
	else if (pid == 0) { // child
		child.toCinCout();
		execl(path.c_str(), path.c_str(), (char*)NULL);
		// if execl failed
		throw std::runtime_error("Error, exec wasn't called: "s + std::strerror(errno));
	}
	else { // parent
		descriptor = std::move(parent);
	}
}

Process::~Process() {
	close();
}

size_t Process::write(const void* data, size_t len) {
	ssize_t written = descriptor.write(data, len);
	if (written == -1) {
		throw std::runtime_error("Error, unable to write: "s + std::strerror(errno));
	}
	return written;
}

void Process::writeExact(const void* data, size_t len){

}

size_t Process::read(void* data, size_t len) {
	ssize_t recieved = descriptor.read(data, len);
	if (recieved == -1) {
		throw std::runtime_error("Error, unable to read: "s + std::strerror(errno));
	}
	return recieved;
}

void Process::readExact(void* data, size_t len) {

}

bool Process::isReadable() const {

}

void Process::closeStdin() {
	descriptor.closeOut();
}

void Process::close() {
	descriptor.close();
}
