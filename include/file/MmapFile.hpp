
#pragma once

#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <cstring>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "http/Fd.hpp"

namespace file {

using namespace std::string_literals;

class MmapFile {
public:
	MmapFile() = delete;
	MmapFile(const std::filesystem::path& file);

	~MmapFile();

	std::size_t size() const;

	void* mmap();

private:
	http::Fd file_fd;
	void* mmap_;
	std::size_t size_;

};

}
