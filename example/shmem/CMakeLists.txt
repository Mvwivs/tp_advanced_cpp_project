
project(async_example)

add_executable(shmem_example
	main.cpp
)

target_link_libraries(shmem_example
	shmem
)
