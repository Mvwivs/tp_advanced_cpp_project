
#pragma once

#include <cstdint>
#include <new>

namespace shmem {

struct ShmemAllocatorState {
	char* start;
	char* end;
};

template <typename T>
class ShmemAllocator {
public:
	using value_type = T;

	ShmemAllocator() noexcept :
		state_(nullptr) {
	}

	ShmemAllocator(ShmemAllocatorState* state) noexcept :
		state_(state) {
	}

	template <typename U>
    friend class ShmemAllocator; // access state of other allocator

	template <typename U>
	ShmemAllocator(const ShmemAllocator<U>& other) noexcept :
		state_(other.state_) {
	}

	ShmemAllocator(const ShmemAllocator& other) noexcept :
		state_(other.state_) {
	}

	[[nodiscard]] T* allocate(std::size_t n) {
		char* res = state_->start;
		if (res + sizeof(T) * n > state_->end) {
			throw std::bad_alloc();
		}
		state_->start += sizeof(T) * n;

		return reinterpret_cast<T*>(res);
	}

	void deallocate(T* p, std::size_t n) noexcept {
		if (state_->start - n * sizeof(T) == reinterpret_cast<char*>(p)) {
			state_->start -= n * sizeof(T);
		}
	}

private:
	ShmemAllocatorState* state_;
};

}
