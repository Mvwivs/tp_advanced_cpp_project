
#include "logger/StdoutLogger.hpp"

namespace logger {
	
void StdoutLogger::flush() {
	std::cout.flush();
}

void StdoutLogger::log_full_message(const std::string& full_message) {
	std::cout << full_message + "\n";
}

}

