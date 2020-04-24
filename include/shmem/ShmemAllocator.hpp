
#pragma once

#include <cstdint>
#include <new>

namespace shmem {

// Allocator state helper class
struct ShmemAllocatorState {
	char* start;	// Pointer to start of free memory
	char* end;		// Pointer to end of allocated memory
};

template <typename T>
class ShmemAllocator {
public:
	using value_type = T;

	ShmemAllocator(ShmemAllocatorState* state) noexcept :
		state_(state) {
	}

	template <typename U>
	friend class ShmemAllocator; // access state of other allocator

	template <typename U>
	ShmemAllocator(const ShmemAllocator<U>& other) noexcept :
		state_(other.state_) {
	}

	// allocate requested space 
	[[nodiscard]] T* allocate(std::size_t n) {
		char* res = state_->start;
		if (res + sizeof(T) * n > state_->end) {
			throw std::bad_alloc();
		}
		state_->start += sizeof(T) * n;

		return reinterpret_cast<T*>(res);
	}

	// deallocate requested space
	void deallocate(T* p, std::size_t n) noexcept {
		if (state_->start - n * sizeof(T) == reinterpret_cast<char*>(p)) {
			state_->start -= n * sizeof(T);
		}
	}

private:
	ShmemAllocatorState* state_;	// Pointer to allocator state
};

}
