
#pragma once

#include <semaphore.h>

namespace shmem {

// Mutex based on linux semaphore with allocator parameter
template <
	template <typename T> 
	typename Alloc
>
class SemaphoreMutex {
public:

	template <typename T>
	SemaphoreMutex(Alloc<T> allocator):
		allocator_(allocator) {

		semaphore_ = allocator_.allocate(1);
		::sem_init(semaphore_, 1, 1);
	}

	~SemaphoreMutex() {
		::sem_destroy(semaphore_);
		allocator_.deallocate(semaphore_, 1);
	}

	// Lock mutex
	void lock() {
		::sem_wait(semaphore_);
	}

	// Unlock mutex
	void unlock() {
		::sem_post(semaphore_);
	}

private:
	sem_t* semaphore_;			// semaphore for access
	Alloc<sem_t> allocator_;	// allocator allowing memory allocation for semaphore
};
}
