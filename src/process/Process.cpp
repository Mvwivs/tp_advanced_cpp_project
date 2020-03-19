
#include "Process.hpp"

namespace process {

Process::Process(const std::string& path) :
	pid(-1) {

	auto [parent, child] = createPipe();

	pid = fork();
	if (pid < 0) {
		throw std::runtime_error("Error, unable to fork: "s + std::strerror(errno));
	}
	else if (pid == 0) { // child
		try {
			close();
			parent.close();
			child.toCinCout();
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
		descriptor = std::move(parent);
	}
}

Process::~Process() {
	close();
	kill(); // maybe wait?
}

size_t Process::write(const void* data, size_t len) {
	ssize_t written = descriptor.write(data, len);
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
	ssize_t recieved = descriptor.read(data, len);
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
	return descriptor.isInAvailable();
}

int Process::join() {
	int returnStatus = EXIT_FAILURE;
	if (pid != -1) {
    	pid_t res = waitpid(pid, &returnStatus, 0);
		if (res != -1) {
			pid = -1;
		}
	}
	return returnStatus;
}

void Process::kill() {
	if (pid != -1) {
		::kill(pid, SIGKILL); // maybe check return status?
	}
}

void Process::closeStdin() {
	descriptor.closeOut();
}

void Process::close() {
	descriptor.close();
}

} // namespace process
