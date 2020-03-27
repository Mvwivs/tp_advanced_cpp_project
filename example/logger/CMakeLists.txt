
project(process_example)

add_executable(logger_example
	main.cpp
)

target_link_libraries(logger_example
		logger
)

target_include_directories(logger_example
	PRIVATE
		${tp_advanced_cpp_SOURCE_DIR}/include
)
