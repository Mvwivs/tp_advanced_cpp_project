
#pragma once

#include <memory>
#include <filesystem>

#include "BaseLogger.hpp"
#include "StdoutLogger.hpp"
#include "StderrLogger.hpp"
#include "FileLogger.hpp"

namespace logger {

// Singleton wrapper for logger
class Logger {
public:
	Logger(const Logger& othrer) = delete;
	Logger& operator=(const Logger& other) = delete;

	// get instance of current logger
	static Logger& get_instance();

	// get currently used logger
	BaseLogger& get_global_logger();

	// reset currently used logger
	void set_global_logger(std::unique_ptr<BaseLogger> new_logger);

private:
	Logger();

private:
	std::unique_ptr<BaseLogger> global_logger_;	// underlying logger
};

// wrapper to create stdout logger
std::unique_ptr<BaseLogger> create_stdout_logger();

// wrapper to create stderr logger
std::unique_ptr<BaseLogger> create_stderr_logger();

// wrapper to create file logger
std::unique_ptr<BaseLogger> create_file_logger(const std::filesystem::path& filename);

// wrapper to write to global logger with debug level
void debug(const std::string& message);

// wrapper to write to global logger with warn level
void warn(const std::string& message);

// wrapper to write to global logger with info level
void info(const std::string& message);

// wrapper to write to global logger with error level
void error(const std::string& message);

} // namespace logger

