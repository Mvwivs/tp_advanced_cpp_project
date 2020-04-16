
#include <stdexcept>
#include <map>
#include <iostream>
#include <mutex>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <unistd.h>

#include "shmem/ShmemAllocator.hpp"
#include "shmem/ShmemMap.hpp"
#include "shmem/Shmem.hpp"

int main() {

	constexpr size_t mmap_size = 1024;

	shmem::Shmem mmap(mmap_size);

	shmem::ShmemMap<int, char> map(mmap.get_allocator<std::pair<int, char>>());

	int pid = fork();
	if (pid != 0) { // child
		map.insert({0, 'a'});
		exit(0);
	}
	else {
		map.insert({0, 'b'});
		map.insert({1, 'b'});
	}
	::waitpid(pid, nullptr, 0);

	for (const auto& [key, val] : map) {
		std::cout << key << " - " << val << std::endl;
	}
	
	return 0;
}
