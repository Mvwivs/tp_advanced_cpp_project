
#pragma once

#include <cstdint>
#include <functional>

#include <unistd.h>


namespace http {

// RAII wraper for file descriptor
struct fd_t {
	int fd;

	fd_t();
	fd_t(int new_fd);
	~fd_t();

	fd_t(const fd_t& other) = delete;
	fd_t& operator=(const fd_t& other) = delete;

	fd_t(fd_t&& other);
	fd_t& operator=(fd_t&& other);

	operator bool() const;

	void close();
};

}

namespace std {

// hash for fd_d
template <> 
struct hash<http::fd_t> {
	size_t operator()(const http::fd_t& x) const {
		return hash<int>()(x.fd); // hash underlying fd
	}
};

}
