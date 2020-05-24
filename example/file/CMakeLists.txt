
project(file_example)

add_executable(file_example
	main.cpp
)

target_link_libraries(file_example
	file
	http
	tcp
	pthread
)
