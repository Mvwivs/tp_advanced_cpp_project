
#pragma once

#include <string>

namespace logger {

// Logging level
enum class Level {
	error = 0, warn, info, debug
};

// Convert logging level to string word
std::string get_loglevel_as_string(Level level);

// create timestamp
std::string get_timestamp();

// Base logger class, allows to log at different levels
class BaseLogger {
public:
	BaseLogger();
	virtual ~BaseLogger() = default;

	// write message with debug level
	void debug(const std::string& message);

	// write message with info level
	void info(const std::string& message);

	// write message with info level
	void warn(const std::string& message);

	// write message with error level
	void error(const std::string& message);

	// set current logging level
	void set_level(Level new_level);

	// get current logging level
	Level level() const;

	// flush logger buffers
	virtual void flush() = 0;

protected:
	// write message to log target
	virtual void log_full_message(const std::string& full_message) = 0;

private:
	// construct full log string with given level and output to log target
	void log(const std::string& message, Level msg_level);

private:
	Level level_;	// current log level
};

}