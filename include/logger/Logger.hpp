
#pragma once

#include <memory>
#include <filesystem>

#include "BaseLogger.hpp"
#include "StdoutLogger.hpp"
#include "StderrLogger.hpp"
#include "FileLogger.hpp"

namespace logger {

class Logger {
public:
	Logger(const Logger& othrer) = delete;
	Logger& operator=(const Logger& other) = delete;

	static Logger& get_instance();

	BaseLogger& get_global_logger();

	void set_global_logger(std::unique_ptr<BaseLogger> new_logger);

private:
	Logger();

private:
	std::unique_ptr<BaseLogger> global_logger_;
};

std::unique_ptr<BaseLogger> create_stdout_logger();

std::unique_ptr<BaseLogger> create_stderr_logger();

std::unique_ptr<BaseLogger> create_file_logger(const std::filesystem::path& filename);

void debug(const std::string& message);

void warn(const std::string& message);

void info(const std::string& message);

void error(const std::string& message);

} // namespace logger

