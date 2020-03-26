
#pragma once

#include <string>
#include <vector>

#include <unistd.h>

namespace process {

// execute program with arguments
int exec(const std::string& path, std::vector<std::string> args);

} // namespace process
