
#include <iostream>
#include <string>
#include <iostream>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "shmem/ShmemMap.hpp"
#include "shmem/Shmem.hpp"

int main() {

	constexpr size_t mmap_size = 1024;

	shmem::Shmem mmap(mmap_size);

	using MyString = std::basic_string<char, std::char_traits<char>, shmem::ShmemAllocator<char>>;

	auto alloc = mmap.get_allocator<std::pair<MyString, char>>();
	shmem::ShmemMap<MyString, char> map(alloc);


	int pid = ::fork();
	if (pid != 0) { // child
		try{
			map.insert_or_assign({MyString("first write", alloc), 'c'});

			map.insert_or_assign({MyString("returned", alloc), 'u'});
			map.insert_or_assign({MyString("erased", alloc), 'c'});
			map.insert_or_assign({MyString("updated", alloc), 'c'});
			map.erase(MyString("erased", alloc));
			char c = map.at(MyString("returned", alloc));
			map.insert_or_assign({MyString("updated", alloc), c});
		}
		catch (const std::exception& e) {
			std::cerr << "Child error: " << e.what() << std::endl;
		}
		exit(0);
	}
	else { // parent
		map.insert_or_assign({MyString("first write", alloc), 'p'});
		map.insert_or_assign({MyString("second write", alloc), 'p'});
	}
	::waitpid(pid, nullptr, 0);

	for (const auto& [key, val] : map) {
		std::cout << key << " - " << val << std::endl;
	}

	mmap.destroy();
	return 0;
}
