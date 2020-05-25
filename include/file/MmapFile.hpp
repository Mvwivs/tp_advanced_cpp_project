
#pragma once

#include <iostream>
#include <filesystem>

#include "http/Fd.hpp"

namespace file {

// Maps file to memory
class MmapFile {
public:
	MmapFile() = delete;
	MmapFile(const std::filesystem::path& file);

	~MmapFile();

	// Get file size
	std::size_t size() const;
	// Get pointer to file in memory
	void* mmap();

private:
	http::Fd file_fd;	// File descriptor for open file
	void* mmap_;		// Pointer to mapped file in memory
	std::size_t size_;	// File size

};

}
