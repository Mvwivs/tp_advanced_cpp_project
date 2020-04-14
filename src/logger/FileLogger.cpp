
#include "logger/FileLogger.hpp"

namespace logger {

FileLogger::FileLogger(const std::filesystem::path& filename):
	log_file(filename) {
}

void FileLogger::flush() {
	log_file.flush();
}

void FileLogger::log_full_message(const std::string& full_message) {
	log_file << full_message << std::endl;
}

}
