
#pragma once

#include <string>

#include <unistd.h>

namespace process {

// execute program with arguments
inline int exec(const std::string& path, std::vector<std::string> args) {
	// copy vector to allow const_cast on it's content later

	std::size_t total_args_cnt = args.size() + 2;

	std::vector<const char*> exec_args(total_args_cnt);

	exec_args[0] = path.c_str();	// program name
	for (std::size_t i = 0; i < args.size(); ++i) {
		exec_args[i + 1] = args[i].c_str();	// set pointers to arguments
	}
	exec_args[total_args_cnt - 1] = (char*)NULL;	// end indicator

	// dangerous const_cast, allow execv to modify string data
	return execv(path.c_str(), const_cast<char* const*>(exec_args.data()));
}

} // namespace process
