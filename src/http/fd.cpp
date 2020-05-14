
#include "http/fd.hpp"

namespace http {

fd_t::fd_t(): 
	fd(-1) {
}

fd_t::fd_t(int new_fd):
	fd(new_fd) {
}

fd_t::fd_t(fd_t&& other):
	fd(std::exchange(other.fd, -1)) {
}

fd_t& fd_t::operator=(fd_t&& other) {
	close();
	fd = std::exchange(other.fd, -1);
	return *this;
}

fd_t::~fd_t() {
	close();
}

fd_t::operator bool() const {
	return fd != -1;
}

void fd_t::close() {
	if (fd != -1) {
		::close(fd);
		fd = -1;
	}
}

}
