
project(tcp_example)

add_executable(tcp_example
	main.cpp
)

target_link_libraries(tcp_example
	# tcp
	pthread
)

target_include_directories(tcp_example
	PRIVATE
		${tp_advanced_cpp_SOURCE_DIR}/include
)