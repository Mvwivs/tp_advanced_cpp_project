
project(process_example)

add_executable(logger_example
	main.cpp
)

target_link_libraries(logger_example
	PRIVATE
		# logger
)
