
add_library(logger
		BaseLogger.cpp
		FileLogger.cpp
		StdoutLogger.cpp
		StderrLogger.cpp
		Logger.cpp
)

target_include_directories(logger
	PRIVATE 
		${tp_advanced_cpp_SOURCE_DIR}/include
)

target_link_libraries(logger
	PRIVATE
		stdc++fs
)
