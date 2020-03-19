
add_library(process
		pipe.cpp
		Process.cpp
)

target_include_directories(process
	PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}
	PUBLIC
		${CMAKE_SOURCE_DIR}/include/process
)
