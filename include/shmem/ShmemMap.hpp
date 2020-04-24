
#pragma once

#include <map>
#include <mutex>

#include "shmem/ShmemAllocator.hpp"
#include "shmem/SemaphoreMutex.hpp"

namespace shmem {

// Shared memory map
template <typename K, typename V>
class ShmemMap {
public:
	using value_type = typename std::map<K, V>::value_type;
	using key_type = typename std::map<K, V>::key_type;
	using mapped_type = typename std::map<K, V>::mapped_type;

	template <typename T>
	using Alloc = shmem::ShmemAllocator<T>;

	using UnderlyingMap = std::map<K, V,
						std::less<K>,
						Alloc<value_type>>;

	using iterator = typename std::map<K, V>::iterator;
	using const_iterator = typename std::map<K, V>::const_iterator;


	ShmemMap(Alloc<value_type> allocator):
		allocator_(allocator),
		mutex(allocator) {

		map_ = allocator_.allocate(1);
		map_ = new (map_) UnderlyingMap{allocator_};
	}

	~ShmemMap() {
		map_->~UnderlyingMap();
		allocator_.deallocate(map_, 1);
	}

	// insert new element or assign if exists
	void insert_or_assign(const value_type& value) {
		std::lock_guard lck(mutex);
		map_->insert_or_assign(value.first, value.second);
	}

	// try to erase elemet
	std::size_t erase(const key_type key) {
		std::lock_guard lck(mutex);
		return map_->erase(key);
	}

	// try to access element
	V at(const key_type key) const {
		std::lock_guard lck(mutex);
		return map_->at(key);
	}

	iterator begin() {
		return map_->begin();
	}
	const_iterator begin() const {
		return map_->begin();
	}
	iterator end() {
		return map_->end();
	}
	const_iterator end() const {
		return map_->end();
	}

private:
	UnderlyingMap* map_;					// map container
	Alloc<UnderlyingMap> allocator_;		// shared memory allocator
	mutable SemaphoreMutex<Alloc> mutex;	// map access control mutex
	
};

}
