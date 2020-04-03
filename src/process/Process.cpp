
#include "process/Process.hpp"

#include <stdexcept>
#include <cstring>
#include <iostream>

#include <sys/wait.h>
#include <unistd.h>

#include "process/exec_utils.hpp"

namespace process {

using namespace std::string_literals; // 's' literal for exceptions

Process::Process(const std::string& path, const std::vector<std::string>& args) :
	pid_(-1) {

	auto [parent, child] = createDuplexPipe();

	pid_ = fork();
	if (pid_ < 0) {
		throw std::runtime_error(std::string("Error, unable to fork: ") + std::strerror(errno));
	}
	else if (pid_ == 0) { // child
		try {
			parent.close();
			child.redirectToStd();
			exec(path, args);
			// if exec failed
			throw std::runtime_error(std::string("Error, exec wasn't called: ") + std::strerror(errno));
		}
		catch (const std::exception& e) {
			child.close();
			std::cerr << e.what() << std::endl;
			std::cerr.flush();
			_exit(EXIT_FAILURE); // maybe print exception?
		}
	}
	else { // parent
		descriptor_ = std::move(parent);
	}
}

Process::Process(Process&& other):
	descriptor_(std::move(other.descriptor_)),
	pid_(other.pid_) {
	other.pid_ = -1;
}

Process& Process::operator=(Process&& other) {
	close();
	descriptor_ = std::move(other.descriptor_);
	pid_ = other.pid_;
	other.pid_ = -1;
	return *this;
}

Process::~Process() {
	close();
	join();
}

size_t Process::write(const void* data, size_t len) {
	ssize_t written = descriptor_.write(data, len);
	if (written == -1) {
		throw std::runtime_error("Error, unable to write: "s + std::strerror(errno));
	}
	return written;
}

void Process::writeExact(const void* data, size_t len){
	size_t written = 0;
	const char* d = static_cast<const char*>(data);
	while (len - written != 0) {
		size_t res = write(d + written, len - written);;
		if (res == 0) {
			throw std::runtime_error("Error, unable to writeExact, EOF: "s + std::strerror(errno));
		}
		written += res;
	}
}

size_t Process::read(void* data, size_t len) {
	ssize_t recieved = descriptor_.read(data, len);
	if (recieved == -1) {
		throw std::runtime_error("Error, unable to read: EOF");
	}
	return recieved;
}

void Process::readExact(void* data, size_t len) {
	size_t recieved = 0;
	char* d = static_cast<char*>(data);
	while (len - recieved != 0) {
		size_t res = read(d + recieved, len - recieved);
		if (res == 0) {
			throw std::runtime_error("Error, unable to readExact: EOF");
		}
		recieved += res;
	}
}

bool Process::isReadable() const {
	return descriptor_.isInAvailable();
}

int Process::join() {
	int returnStatus = EXIT_FAILURE;
	if (pid_ != -1) {
		if (waitpid(pid_, &returnStatus, 0) != -1) {
			pid_ = -1;
		}
	}
	return returnStatus;
}

void Process::kill() {
	if (pid_ != -1) {
		::kill(pid_, SIGKILL); // maybe check return status?
	}
}

void Process::closeStdin() {
	descriptor_.closeOut();
}

void Process::close() {
	descriptor_.close();
}

} // namespace process
