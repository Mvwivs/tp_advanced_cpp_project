
add_library(tcp
		connection_utils.cpp
		Connection.cpp
		Server.cpp
)

target_include_directories(tcp
	PRIVATE 
		${tp_advanced_cpp_SOURCE_DIR}/include
)
