
#include "process/exec_utils.hpp"

#include <unistd.h>

namespace process {

int exec(std::string path, std::vector<std::string> args) {
	// copy path and args because execv is able to modify them

	std::size_t total_args_cnt = args.size() + 2;

	std::vector<char*> exec_args(total_args_cnt);
	exec_args.push_back(path.data()); 	// program name
	for (std::string& s : args) {
		exec_args.push_back(s.data()); // set pointers to arguments
	}
	exec_args.push_back(nullptr); // end indicator

	return execv(path.data(), exec_args.data());
}

}
