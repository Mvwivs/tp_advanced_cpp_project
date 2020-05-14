
add_library(http
		HTTP.cpp
		fd.cpp
		Worker.cpp
		Server.cpp
		ClientState.cpp
)

target_link_libraries(http
	PRIVATE
		tcp
		coroutine
		logger
)

target_include_directories(http
	PUBLIC 
		${tp_advanced_cpp_SOURCE_DIR}/include
)
