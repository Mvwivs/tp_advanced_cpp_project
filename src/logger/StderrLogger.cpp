
#include "logger/StderrLogger.hpp"

namespace logger {

void StderrLogger::flush() {
	std::cerr.flush();
}

void StderrLogger::log_full_message(const std::string& full_message) {
	std::cerr << full_message << std::endl;
}

}
