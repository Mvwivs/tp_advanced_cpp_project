
project(async_example)

add_executable(async_example
	main.cpp
)

target_link_libraries(async_example
	tcp
	pthread
)
