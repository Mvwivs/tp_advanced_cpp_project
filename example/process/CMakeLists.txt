
project(process_example)

add_executable(process_example
	main.cpp
)

target_link_libraries(process_example
	PRIVATE
		process
)
