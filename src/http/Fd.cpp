
#include "http/Fd.hpp"

namespace http {

Fd::Fd(): 
	fd(-1) {
}

Fd::Fd(int new_fd):
	fd(new_fd) {
}

Fd::Fd(Fd&& other):
	fd(std::exchange(other.fd, -1)) {
}

Fd& Fd::operator=(Fd&& other) {
	close();
	fd = std::exchange(other.fd, -1);
	return *this;
}

Fd::~Fd() {
	close();
}

Fd::operator bool() const {
	return fd != -1;
}

void Fd::close() {
	if (fd != -1) {
		::close(fd);
		fd = -1;
	}
}

}
