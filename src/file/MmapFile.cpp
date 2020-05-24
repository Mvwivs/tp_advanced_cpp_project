
#include "file/MmapFile.hpp"

#include <stdexcept>
#include <cstring>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

namespace file {

using namespace std::string_literals;

MmapFile::MmapFile(const std::filesystem::path& file):
	file_fd(::open(file.c_str(), O_RDONLY, 0)),
	mmap_(nullptr) {
	
	if (file_fd == -1) {
		throw std::runtime_error("Unable to open file: "s + std::strerror(errno));
	}

	struct stat st;
	int res = ::fstat(file_fd.fd, &st) == -1;
	if (res == -1) {
		throw std::runtime_error("Unable to get file size: "s + std::strerror(errno));
	}
	size_ = static_cast<std::size_t>(st.st_size);

	mmap_ = ::mmap(nullptr, size_, PROT_READ, MAP_PRIVATE, file_fd.fd, 0);
	if(mmap_ == MAP_FAILED) {
		throw std::runtime_error("Memory map failed: "s + std::strerror(errno));
	}
}

MmapFile::~MmapFile() {
	munmap(mmap_, size_);
}

std::size_t MmapFile::size() const {
	return size_;
}

void* MmapFile::mmap() {
	return mmap_;
}

}
