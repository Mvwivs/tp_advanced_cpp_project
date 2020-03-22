
#pragma once

#include <string>
#include <array>

#include <unistd.h>

namespace process {

namespace details {

// Default way to convert data to string
template <typename T>
std::string as_string(T t) {
	return std::to_string(t);
}

// Specialize as_string for const char* (std::to_string doesn't support this type)
inline std::string as_string(const char* t) {
	return std::string(t);
}

// Specialize as_string for std::string (std::to_string doesn't support this type)
inline std::string as_string(const std::string& t) {
	return t;
}

// End recursion, no arguments to convert left
template <typename Iterator>
void to_string_array(Iterator it) {
	return;
}

// Convert arguments to strings and save to container
template <typename Iterator, typename T, typename... Rest>
void to_string_array(Iterator it, T t, Rest... rest) {
	*it = as_string(t);					// convert first argument from list
	to_string_array(++it, rest...);		// convert other argumetns
}

} // namespace details

// Call execv with variable number of arguments, all converted to string
template <typename... Args>
int exec(const std::string& path, Args... args) {

	constexpr std::size_t input_arg_count = sizeof...(Args);
	std::array<std::string, input_arg_count> input_arguments_as_string;
	details::to_string_array(input_arguments_as_string.begin(), args...);

	constexpr std::size_t exec_arg_count = input_arg_count + 2; // two more elements added
	std::array<const char*, exec_arg_count> exec_args;

	exec_args[0] = path.c_str();	// program name
	for (std::size_t i = 0; i < input_arg_count; ++i) {
		exec_args[i + 1] = input_arguments_as_string[i].c_str();	// set pointers to arguments
	}
	exec_args[exec_arg_count - 1] = (char*)NULL;	// end indicator

	// dangerous const_cast, allow execv to modify string data
	return execv(path.c_str(), const_cast<char* const*>(exec_args.data()));
}

} // namespace process
