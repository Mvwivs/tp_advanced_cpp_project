
#pragma once

#include <iostream>
#include <filesystem>

#include "http/Fd.hpp"

namespace file {

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
