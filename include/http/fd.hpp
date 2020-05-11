
#pragma once

#include <utility>

#include <unistd.h>

namespace http {

struct fd_t {
	int fd;
	
	fd_t(): 
		fd(-1) {
	}

	fd_t(int new_fd):
		fd(new_fd) {
	}

	fd_t(const fd_t& other) = delete;
	fd_t& operator=(const fd_t& other) = delete;

	fd_t(fd_t&& other):
		fd(std::exchange(other.fd, -1)) {
	}

	fd_t& operator=(fd_t&& other) {
		close();
		fd = std::exchange(other.fd, -1);
		return *this;
	}

	~fd_t() {
		close();
	}

	operator bool() const {
		return fd != -1;
	}

	void close() {
		if (fd != -1) {
			::close(fd);
			fd = -1;
		}
	}
};

}

namespace std {

template <> 
struct hash<http::fd_t> {
	size_t operator()(const http::fd_t& x) const {
		return hash<int>()(x.fd);
	}
};

}
