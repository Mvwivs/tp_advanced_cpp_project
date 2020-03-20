
#include "process/pipe.hpp"

namespace process {

using namespace std::string_literals; // 's' literal for exceptions

void closeDescriptor(int& fd) {
	if (fd != -1) {
		::close(fd);
	}
	fd = -1;
}

DuplexDescriptor::DuplexDescriptor() :
	in_(-1),
	out_(-1) {
}

DuplexDescriptor::DuplexDescriptor(int input, int output) :
	in_(input),
	out_(output) {
}

DuplexDescriptor::DuplexDescriptor(DuplexDescriptor&& other):
	in_(other.in_),
	out_(other.out_) {
	other.in_ = -1;
	other.out_ = -1;
}

DuplexDescriptor& DuplexDescriptor::operator=(DuplexDescriptor&& other) {
	in_ = other.in_;
	out_ = other.out_;
	other.in_ = -1;
	other.out_ = -1;
	return *this;
}

DuplexDescriptor::~DuplexDescriptor() {
	close();
};

void DuplexDescriptor::closeIn() {
	closeDescriptor(in_);
}

void DuplexDescriptor::closeOut() {
	closeDescriptor(out_);
}

bool DuplexDescriptor::isInAvailable() const {
	return fcntl(in_, F_GETFD) != -1 || errno != EBADF;
}

size_t DuplexDescriptor::write(const void* data, size_t len) {
	return ::write(out_, data, len);
}

size_t DuplexDescriptor::read(void* data, size_t len) {
	return ::read(in_, data, len);
}

void DuplexDescriptor::redirectToStd() {
	if (dup2(in_, STDIN_FILENO) < 0) {	// replace stdin
		throw std::runtime_error("Error, unable to redirect to stdin: "s + std::strerror(errno));
	}
	closeDescriptor(in_);
	if (dup2(out_, STDOUT_FILENO) < 0) {	// replace stdout
		throw std::runtime_error("Error, unable to redirect to stdout: "s + std::strerror(errno));
	}
	closeDescriptor(out_);
}

void DuplexDescriptor::close() {
	closeDescriptor(in_);
	closeDescriptor(out_);
}

DuplexPipe createDuplexPipe() {	// TODO: replace with factory
	int readpipe[2] = {-1, -1};
	int writepipe[2] = {-1, -1};
	if (pipe(readpipe) < 0 || pipe(writepipe) < 0) { // create two pipes
		for (int fd : readpipe) {
			closeDescriptor(fd);
		}
		for (int fd : writepipe) {
			closeDescriptor(fd);
		}
		throw std::runtime_error("Error, unable to create pipes: "s + std::strerror(errno));
	}
	return {
		DuplexDescriptor{ readpipe[0], writepipe[1] },
		DuplexDescriptor{ writepipe[0], readpipe[1] }
	};
}

} // namespace process
