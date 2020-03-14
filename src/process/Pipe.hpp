
#pragma once

#include <stdexcept>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>

using namespace std::string_literals;

class Descriptor {
public:
	virtual size_t write(const void* data, size_t len) = 0;
	virtual size_t read(void* data, size_t len) = 0;
	virtual void close() = 0;
};

class DoubleDescriptor : public Descriptor {
public:
	explicit DoubleDescriptor():
		in(-1),
		out(-1) {
	}

	DoubleDescriptor(const DoubleDescriptor& other) = delete;
	DoubleDescriptor& operator=(const DoubleDescriptor& other) = delete;

	DoubleDescriptor(DoubleDescriptor&& other):
		in(other.in),
		out(other.out) {
		other.in = -1;
		other.out = -1;
	}

	DoubleDescriptor& operator=(DoubleDescriptor&& other) {
		in = std::move(other.in);
		out = std::move(other.out);
		other.in = -1;
		other.out = -1;
	}

	~DoubleDescriptor() {
		close();
	};

	explicit DoubleDescriptor(int input, int output) :
		in(input),
		out(output) {
	}

	void closeIn() {
		close_d(in);
	}

	void closeOut() {
		close_d(out);
	}

	bool isInAvailable() const {
		return fcntl(in, F_GETFD) != -1 || errno != EBADF;
	}

	size_t write(const void* data, size_t len) override {
		return ::write(out, data, len);
	}

	size_t read(void* data, size_t len) override {
		return ::read(in, data, len);
	}

	void toCinCout() {
		if (dup2(in, STDIN_FILENO) < 0) {
			throw std::runtime_error("Error, unable to redirect to cin: "s + std::strerror(errno));
		}
		close_d(in);
		if (dup2(out, STDOUT_FILENO) < 0) {
			throw std::runtime_error("Error, unable to redirect to cout: "s + std::strerror(errno));
		}
		close_d(out);
	}

	void close() override {
		close_d(in);
		close_d(out);
	}

private:
	static void close_d(int& fd) {
		if (fd != -1) {
			::close(fd);
		}
		fd = -1;
	}

private:
	int in;
	int out;

};

using Pipe = std::pair<DoubleDescriptor, DoubleDescriptor>;

inline Pipe createPipe() {	// TODO: replace with factory
	int readpipe[2] = {-1, -1};
	int writepipe[2] = {-1, -1};
	if (pipe(readpipe) < 0 || pipe(writepipe) < 0) {
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
		DoubleDescriptor{readpipe[0], writepipe[1]},
		DoubleDescriptor{writepipe[0], readpipe[1]}
	};
}
