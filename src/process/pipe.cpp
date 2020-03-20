
#include "pipe.hpp"

namespace process {

using namespace std::string_literals; // 's' literal for exceptions

DuplexDescriptor::DuplexDescriptor() :
	in(-1),
	out(-1) {
}

DuplexDescriptor::DuplexDescriptor(int input, int output) :
	in(input),
	out(output) {
}

DuplexDescriptor::DuplexDescriptor(DuplexDescriptor&& other):
	in(other.in),
	out(other.out) {
	other.in = -1;
	other.out = -1;
}

DuplexDescriptor& DuplexDescriptor::operator=(DuplexDescriptor&& other) {
	in = other.in;
	out = other.out;
	other.in = -1;
	other.out = -1;
	return *this;
}

DuplexDescriptor::~DuplexDescriptor() {
	close();
};

void DuplexDescriptor::closeIn() {
	closeDescriptor(in);
}

void DuplexDescriptor::closeOut() {
	closeDescriptor(out);
}

bool DuplexDescriptor::isInAvailable() const {
	return fcntl(in, F_GETFD) != -1 || errno != EBADF;
}

size_t DuplexDescriptor::write(const void* data, size_t len) {
	return ::write(out, data, len);
}

size_t DuplexDescriptor::read(void* data, size_t len) {
	return ::read(in, data, len);
}

void DuplexDescriptor::redirectToStd() {
	if (dup2(in, STDIN_FILENO) < 0) {	// replace stdin
		throw std::runtime_error("Error, unable to redirect to stdin: "s + std::strerror(errno));
	}
	closeDescriptor(in);
	if (dup2(out, STDOUT_FILENO) < 0) {	// replace stdout
		throw std::runtime_error("Error, unable to redirect to stdout: "s + std::strerror(errno));
	}
	closeDescriptor(out);
}

void DuplexDescriptor::close() {
	closeDescriptor(in);
	closeDescriptor(out);
}

void DuplexDescriptor::closeDescriptor(int& fd) {
	if (fd != -1) {
		::close(fd);
	}
	fd = -1;
}

Pipe createPipe() {	// TODO: replace with factory
	int readpipe[2] = {-1, -1};
	int writepipe[2] = {-1, -1};
	if (pipe(readpipe) < 0 || pipe(writepipe) < 0) { // create two pipes
		for (int fd : readpipe) {
			if (fd != -1) {
				::close(fd);
			}
		}
		for (int fd : writepipe) {
			if (fd != -1) {
				::close(fd);
			}
		}
		throw std::runtime_error("Error, unable to create pipes: "s + std::strerror(errno));
	}
	return {
		DuplexDescriptor{ readpipe[0], writepipe[1] },
		DuplexDescriptor{ writepipe[0], readpipe[1] }
	};
}

} // namespace process
