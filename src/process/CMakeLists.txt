
add_library(process
		pipe.cpp
		Process.cpp
		exec_utils.cpp
)

target_include_directories(process
	PUBLIC 
		${tp_advanced_cpp_SOURCE_DIR}/include
)
