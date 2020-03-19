
project(process_example)

add_subdirectory(process)

add_executable(process_example
	main.cpp
)

target_include_directories(process_example
	PRIVATE
		${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(process_example
	PRIVATE
		process
)
