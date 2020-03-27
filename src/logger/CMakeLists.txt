
add_library(logger
		BaseLogger.cpp
		FileLogger.cpp
		StdoutLogger.cpp
		StderrLogger.cpp
		Logger.cpp
)

target_include_directories(logger
	PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(logger
	PRIVATE
		stdc++fs
)
