
#include "shmem/Shmem.hpp"

#include <stdexcept>

#include <sys/mman.h>

namespace shmem {

Shmem::Shmem(const std::size_t size):
	mmap_(nullptr),
	size_(size),
	state_(nullptr) {
	mmap_ = ::mmap(nullptr, size_, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (mmap == MAP_FAILED) {
		throw std::runtime_error("Error: unable to create mmap");
	}
	state_ = static_cast<shmem::ShmemAllocatorState*>(mmap_);
	state_->start = static_cast<char*>(mmap_) + sizeof(*state_);
	state_->end = static_cast<char*>(mmap_) + size_;
}

Shmem::~Shmem() {
	if (mmap_) {
		::munmap(mmap_, size_);
		mmap_ = nullptr;
		state_ = nullptr;
	}
}

}
