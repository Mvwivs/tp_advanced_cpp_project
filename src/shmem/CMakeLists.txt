
add_library(shmem 
	Shmem.cpp
)

target_include_directories(shmem
	PUBLIC 
		${tp_advanced_cpp_SOURCE_DIR}/include
)

target_link_libraries(shmem
	PRIVATE
		pthread
)
