
add_executable(process_example
	main.cpp
)

target_link_libraries(process_example
	process
)

target_include_directories(process_example
	PRIVATE
		${tp_advanced_cpp_SOURCE_DIR}/include
)
