
#pragma once

#include <string>
#include <stdexcept>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace logger {

enum class Level {
	error = 0, warn, info, debug
};

std::string get_loglevel_as_string(Level level);

std::string get_timestamp();

class BaseLogger {
public:
	BaseLogger();

	virtual ~BaseLogger() = default;

	void debug(const std::string& message);

	void info(const std::string& message);

	void warn(const std::string& message);

	void error(const std::string& message);

	void set_level(Level new_level);

	Level level() const;

	virtual void flush() = 0;

protected:
	virtual void log_full_message(const std::string& full_message) = 0;

private:
	void log(const std::string& message, Level msg_level);

private:
	Level level_;
};

}