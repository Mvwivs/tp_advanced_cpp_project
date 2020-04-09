
project(tcp_example)

add_executable(tcp_example
	main.cpp
)

target_link_libraries(tcp_example
	tcp
)
