
#pragma once

#include <string>
#include <vector>

namespace process {

// execute program with arguments
int exec(std::string path, std::vector<std::string> args);

} // namespace process
