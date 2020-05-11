
#pragma once

#include <cstdint>
#include <new>

#include <semaphore.h>

namespace shmem {

// Allocator state helper class
struct ShmemAllocatorState {
	char* start;	// Pointer to start of free memory
	char* end;		// Pointer to end of allocated memory
	sem_t mtx_;		// Allocation synchronization
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
		::sem_wait(&state_->mtx_);
		char* res = state_->start;
		if (res + sizeof(T) * n > state_->end) {
			::sem_post(&state_->mtx_);
			throw std::bad_alloc();
		}
		state_->start += sizeof(T) * n;
		::sem_post(&state_->mtx_);
		return reinterpret_cast<T*>(res);
	}

	// deallocate requested space
	void deallocate(T* p, std::size_t n) noexcept {
		::sem_wait(&state_->mtx_);
		if (state_->start - n * sizeof(T) == reinterpret_cast<char*>(p)) {
			state_->start -= n * sizeof(T);
		}
		::sem_post(&state_->mtx_);
	}

private:
	ShmemAllocatorState* state_;	// Pointer to allocator state
};

}
