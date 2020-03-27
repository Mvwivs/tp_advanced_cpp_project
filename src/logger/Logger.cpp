
#include "logger/Logger.hpp"

namespace logger {

Logger::Logger():
	global_logger_(std::make_unique<StdoutLogger>()) {
}

Logger& Logger::get_instance() {
	static Logger instance;
	return instance;
}

BaseLogger& Logger::get_global_logger() {
	return *global_logger_.get();
}

void Logger::set_global_logger(std::unique_ptr<BaseLogger> new_logger) {
	global_logger_ = std::move(new_logger);
}

std::unique_ptr<BaseLogger> create_stdout_logger() {
	return std::make_unique<StdoutLogger>();
}

std::unique_ptr<BaseLogger> create_stderr_logger() {
	return std::make_unique<StderrLogger>();
}

std::unique_ptr<BaseLogger> create_file_logger(const std::filesystem::path& filename) {
	return std::make_unique<FileLogger>(filename);
}

void debug(const std::string& message) {
	Logger::get_instance().get_global_logger().debug(message);
}

void warn(const std::string& message) {
	Logger::get_instance().get_global_logger().warn(message);
}

void info(const std::string& message) {
	Logger::get_instance().get_global_logger().info(message);
}

void error(const std::string& message) {
	Logger::get_instance().get_global_logger().error(message);
}

} // namespace logger

