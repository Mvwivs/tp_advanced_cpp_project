
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
			map.insert_or_assign({"first write", 'c'});

			map.insert_or_assign({"returned", 'u'});
			map.insert_or_assign({"erased", 'c'});
			map.insert_or_assign({"updated", 'c'});
			map.erase("erased");
			char c = map.at("returned");
			map.insert_or_assign({"updated", c});
		}
		catch (const std::exception& e) {
			std::cerr << "Child error: " << e.what() << std::endl;
		}
		exit(0);
	}
	else { // parent
		map.insert_or_assign({"first write", 'p'});
		map.insert_or_assign({"second write", 'p'});
	}
	::waitpid(pid, nullptr, 0);

	for (const auto& [key, val] : map) {
		std::cout << key << " - " << val << std::endl;
	}
	
	return 0;
}
