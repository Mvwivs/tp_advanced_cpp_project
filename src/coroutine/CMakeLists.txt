
add_library(coroutine
		coroutine.cpp
)

target_include_directories(coroutine
	PUBLIC 
		${tp_advanced_cpp_SOURCE_DIR}/include
)
