
#pragma once

#include <cstdint>
#include <functional>

#include <unistd.h>


namespace http {

// RAII wraper for file descriptor
struct Fd {
	int fd;

	Fd();
	Fd(int new_fd);
	~Fd();

	Fd(const Fd& other) = delete;
	Fd& operator=(const Fd& other) = delete;

	Fd(Fd&& other);
	Fd& operator=(Fd&& other);

	operator bool() const;

	void close();
};

}

namespace std {

// hash for fd_d
template <> 
struct hash<http::Fd> {
	size_t operator()(const http::Fd& x) const {
		return hash<int>()(x.fd); // hash underlying fd
	}
};

}
