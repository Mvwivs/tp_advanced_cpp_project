
#include "process/Process.hpp"

namespace process {

using namespace std::string_literals; // 's' literal for exceptions

Process::Process(const std::string& path) :
	pid_(-1) {

	auto [parent, child] = createPipe();

	pid_ = fork();
	if (pid_ < 0) {
		throw std::runtime_error("Error, unable to fork: "s + std::strerror(errno));
	}
	else if (pid_ == 0) { // child
		try {
			close();
			parent.close();
			child.redirectToStd();
			execl(path.c_str(), path.c_str(), (char*)NULL);
			// if execl failed
			throw std::runtime_error("Error, exec wasn't called: "s + std::strerror(errno));
		}
		catch (const std::exception& e) {
			child.close();
			exit(EXIT_FAILURE); // maybe print exception?
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
	descriptor_ = std::move(other.descriptor_);
	pid_ = other.pid_;
	other.pid_ = -1;
	return *this;
}

Process::~Process() {
	close();
	kill(); // maybe wait?
}

size_t Process::write(const void* data, size_t len) {
	ssize_t written = descriptor_.write(data, len);
	if (written == -1) {
		throw std::runtime_error("Error, unable to write: "s + std::strerror(errno));
	}
	return written;
}

void Process::writeExact(const void* data, size_t len){
	ssize_t written = 0;
	const char* d = static_cast<const char*>(data);
	while (len - written != 0) {
		written += write(d + written, len - written);
	}
}

size_t Process::read(void* data, size_t len) {
	ssize_t recieved = descriptor_.read(data, len);
	if (recieved == -1) {
		throw std::runtime_error("Error, unable to read: "s + std::strerror(errno));
	}
	return recieved;
}

void Process::readExact(void* data, size_t len) {
	ssize_t recieved = 0;
	char* d = static_cast<char*>(data);
	while (len - recieved != 0) {
		recieved += read(d + recieved, len - recieved); // maybe check for eof?
	}
}

bool Process::isReadable() const {
	return descriptor_.isInAvailable();
}

int Process::join() {
	int returnStatus = EXIT_FAILURE;
	if (pid_ != -1) {
    	pid_t res = waitpid(pid_, &returnStatus, 0);
		if (res != -1) {
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
