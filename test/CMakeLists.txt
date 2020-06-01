
add_executable(echoer
	echo.cpp
)

add_executable(generator
	generate.cpp
)

target_link_libraries(generator
	file
	http
)
