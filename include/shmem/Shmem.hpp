
#pragma once

#include <cstdint>

#include "shmem/ShmemAllocator.hpp"

namespace shmem {

// Shared memory pool with allocator
class Shmem {
public:
	Shmem() = delete;
	Shmem(const std::size_t size);

	~Shmem();

	// Get allocator for shared memory
	template <typename T>
	ShmemAllocator<T> get_allocator();

private:
	void* mmap_;						// Pointer to shared memory start
	std::size_t size_;					// Shared memory size
	shmem::ShmemAllocatorState* state_;	// State of allocator
};

template <typename T>
ShmemAllocator<T> Shmem::get_allocator() {
	ShmemAllocator<T> alloc(state_);
	return alloc;
}

}
