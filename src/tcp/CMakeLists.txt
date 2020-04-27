
add_library(tcp
		connection_utils.cpp
		Connection.cpp
		Server.cpp
		AsyncServer.cpp
		AsyncConnection.cpp
		connection_state.cpp
)

target_include_directories(tcp
	PUBLIC 
		${tp_advanced_cpp_SOURCE_DIR}/include
)
