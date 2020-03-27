
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

inline std::string get_loglevel_as_string(Level level) {
	switch (level) {
		case Level::error:
			return "ERROR";
		case Level::warn:
			return "WARN";
		case Level::info:
			return "INFO";
		case Level::debug:
			return "DEBUG";
		default:
			throw std::runtime_error("Unknown log level");
	}
}

inline std::string get_timestamp() {
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y.%m.%d-%H:%M:%S");
	return ss.str();
}

class BaseLogger {
public:
	BaseLogger() : level_(Level::debug) {
	}

	virtual ~BaseLogger() = default;

	void debug(const std::string& message) {
		log(message, Level::debug);
	}

	void info(const std::string& message) {
		log(message, Level::info);
	}

	void warn(const std::string& message) {
		log(message, Level::warn);
	}

	void error(const std::string& message) {
		log(message, Level::error);
	}

	void set_level(Level new_level) {
		level_ = new_level;
	}

	Level level() const {
		return level_;
	}

	virtual void flush() = 0;

protected:
	virtual void log_full_message(const std::string& full_message) = 0;

private:
	void log(const std::string& message, Level msg_level) {
		if (msg_level > level_) {
			return;
		}
		std::stringstream ss;
		ss << '[' << get_timestamp() << ']' << '\t'
			<< '[' << get_loglevel_as_string(msg_level) << ']' << '\t'
			<< message;
		log_full_message(ss.str());
	};

private:
	Level level_;
};

}