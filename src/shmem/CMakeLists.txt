
add_library(shmem INTERFACE
)

target_include_directories(shmem
	INTERFACE 
		${tp_advanced_cpp_SOURCE_DIR}/include
)
