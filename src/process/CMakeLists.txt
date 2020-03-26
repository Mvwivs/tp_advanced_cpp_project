
add_library(process
		pipe.cpp
		Process.cpp
		exec_utils.cpp
)

target_include_directories(process
	PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}
)
