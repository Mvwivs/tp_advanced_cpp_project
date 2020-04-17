
#pragma once

#include <cstdint>

#include "shmem/ShmemAllocator.hpp"

namespace shmem {

class Shmem {
public:
	Shmem() = delete;
	Shmem(const std::size_t size);

	~Shmem();

	template <typename T>
	ShmemAllocator<T> get_allocator();

private:
	void* mmap_;
	std::size_t size_;
	shmem::ShmemAllocatorState* state_;
};

template <typename T>
ShmemAllocator<T> Shmem::get_allocator() {
	ShmemAllocator<T> alloc(state_);
	return alloc;
}

}
