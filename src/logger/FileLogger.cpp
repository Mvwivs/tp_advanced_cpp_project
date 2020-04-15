
#include "logger/FileLogger.hpp"

namespace logger {

FileLogger::FileLogger(const std::filesystem::path& filename):
	log_file(filename) {
	if (!log_file.is_open()) {
		throw LoggerException("Error: unable to open file " + filename.string());
	}
}

void FileLogger::flush() {
	log_file.flush();
}

void FileLogger::log_full_message(const std::string& full_message) {
	log_file << full_message << std::endl;
	if (!log_file) {
		throw LoggerException("Error: logger wasn't able to write to file");
	}
}

}
