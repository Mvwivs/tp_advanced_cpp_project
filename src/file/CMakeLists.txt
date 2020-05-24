
add_library(file
		DbServer.cpp
		Index.cpp
		MmapFile.cpp
)

target_link_libraries(file
	PRIVATE
		http
		stdc++fs
)

target_include_directories(file
	PUBLIC 
		${tp_advanced_cpp_SOURCE_DIR}/include
)
