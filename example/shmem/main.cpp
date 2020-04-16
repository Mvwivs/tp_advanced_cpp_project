
#include <iostream>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "shmem/ShmemMap.hpp"
#include "shmem/Shmem.hpp"

int main() {

	constexpr size_t mmap_size = 1024;

	shmem::Shmem mmap(mmap_size);

	shmem::ShmemMap<int, char> map(mmap.get_allocator<std::pair<int, char>>());

	int pid = ::fork();
	if (pid != 0) { // child
		map.insert_or_assign({0, 'c'});
		map.insert_or_assign({1, 'c'});

		map.insert_or_assign({2, 'u'});
		map.insert_or_assign({3, 'c'});
		map.insert_or_assign({4, 'c'});
		map.erase(3);
		char c = map.at(2);
		map.insert_or_assign({3, c});
		exit(0);
	}
	else { // parent
		map.insert_or_assign({0, 'p'});
		map.insert_or_assign({1, 'p'});
	}
	::waitpid(pid, nullptr, 0);

	for (const auto& [key, val] : map) {
		std::cout << key << " - " << val << std::endl;
	}
	
	return 0;
}
