
#pragma once

#include <semaphore.h>

namespace shmem {

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

	void lock() {
		::sem_wait(semaphore_);
	}

	void unlock() {
		::sem_post(semaphore_);
	}

private:
	sem_t* semaphore_;
	Alloc<sem_t> allocator_;
};
}
