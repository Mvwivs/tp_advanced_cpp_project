
#include "process/pipe.hpp"

#include <stdexcept>
#include <cstring>
#include <string>

#include <unistd.h>
#include <fcntl.h>

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

std::size_t DuplexDescriptor::write(const void* data, std::size_t len) {
	return ::write(out_, data, len);
}

std::size_t DuplexDescriptor::read(void* data, std::size_t len) {
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

DuplexPipe createDuplexPipe() {
	/*
		|    (1)     |      ---- [[0] first_pipe  [1]] --->     |    (2)     |
		| descriptor |      <--- [[1] second_pipe [0]] ----     | descriptor |
	*/
	int first_pipe[2] = {-1, -1};
	int second_pipe[2] = {-1, -1};
	if (pipe(first_pipe) < 0 || pipe(second_pipe) < 0) { // create two pipes
		closeDescriptor(first_pipe[0]);
		closeDescriptor(first_pipe[1]);
		closeDescriptor(second_pipe[0]);
		closeDescriptor(second_pipe[1]);
		throw std::runtime_error("Error, unable to create pipes: "s + std::strerror(errno));
	}
	return {
		DuplexDescriptor{ first_pipe[0], second_pipe[1] },	// (1) descriptor
		DuplexDescriptor{ second_pipe[0], first_pipe[1] }	// (2) descriptor
	};
}

} // namespace process
