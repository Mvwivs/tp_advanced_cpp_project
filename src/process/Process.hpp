
#pragma once

#include <stdexcept>
#include <string>
#include <cstring>

#include <sys/types.h>
#include <unistd.h>

using namespace std::string_literals;

class Process {
public:
	explicit Process(const std::string& path);
	~Process();

	size_t write(const void* data, size_t len);
	void writeExact(const void* data, size_t len);
	size_t read(void* data, size_t len);
	void readExact(void* data, size_t len);

	bool isReadable() const;
	void closeStdin();

	void close();
private:
	int readpipe[2];
	int writepipe[2];

	int& write_fd;
	int& read_fd;
};

Process::Process(const std::string& path):
	readpipe{-1,-1},
	writepipe{-1, -1},
	read_fd(readpipe[0]),
	write_fd(writepipe[1]) {
	if (pipe(readpipe) < 0 || pipe(writepipe) < 0) {
		close();
		throw std::runtime_error("Error, unable to create pipes: "s + std::strerror(errno));
	}
	int& child_write = readpipe[1];
	int& child_read = writepipe[0];

	pid_t pid = fork();
	if (pid < 0) {
		close();
		throw std::runtime_error("Error, unable to fork: "s + std::strerror(errno));
	}
	else if (pid == 0) { // child
		::close(write_fd);
		::close(read_fd);
		read_fd = -1;
		write_fd = -1;
		if (dup2(child_read, STDIN_FILENO) < 0) {
			close();
			exit(EXIT_FAILURE);
		}
		::close(child_read);
		child_read = -1;
		if (dup2(child_write, STDOUT_FILENO) < 0) {
			close();
			exit(EXIT_FAILURE);
		}
		::close(child_write);
		child_write = -1;
		execl(path.c_str(), path.c_str(), (char*) NULL);
		// if execl failed
		close();
		exit(EXIT_FAILURE);
	}
	else { // parent
		::close(child_read);
		child_read = -1;
		::close(child_write);
		child_write = -1;
	}
}

Process::~Process() {
	close();
}

size_t Process::write(const void* data, size_t len) {
	ssize_t written = ::write(write_fd, data, len);
	if (written == -1) {
		throw std::runtime_error("Error, unable to write: "s + std::strerror(errno));
	}
	return written;
}

void Process::writeExact(const void* data, size_t len){

}

size_t Process::read(void* data, size_t len) {
	ssize_t recieved = ::read(read_fd, data, len);
	if (recieved == -1) {
		throw std::runtime_error("Error, unable to read: "s + std::strerror(errno));
	}
	return recieved;
}

void Process::readExact(void* data, size_t len) {

}

bool Process::isReadable() const {
	return (read_fd != -1);
}

void Process::closeStdin() {
	::close(write_fd);
	write_fd = -1;
}

void Process::close() {
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
}
