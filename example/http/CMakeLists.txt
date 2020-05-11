
project(http_example)

add_executable(http_example
	main.cpp
)

target_link_libraries(http_example
	http
	tcp
	pthread
)
