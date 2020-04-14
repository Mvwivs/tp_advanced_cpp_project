
#include "logger/BaseLogger.hpp"

#include <stdexcept>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace logger {

std::string get_loglevel_as_string(Level level) {
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
			return "UNKNOWN";
	}
}

std::string get_timestamp() {
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y.%m.%d-%H:%M:%S");
	return ss.str();
}


BaseLogger::BaseLogger() : level_(Level::debug) {
}

void BaseLogger::debug(const std::string& message) {
	log(message, Level::debug);
}

void BaseLogger::info(const std::string& message) {
	log(message, Level::info);
}

void BaseLogger::warn(const std::string& message) {
	log(message, Level::warn);
}

void BaseLogger::error(const std::string& message) {
	log(message, Level::error);
}

void BaseLogger::set_level(Level new_level) {
	level_ = new_level;
}

Level BaseLogger::level() const {
	return level_;
}

void BaseLogger::log(const std::string& message, Level msg_level) {
	if (msg_level > level_) {
		return;
	}
	std::stringstream ss;
	ss << '[' << get_timestamp() << ']' << '\t'
		<< '[' << get_loglevel_as_string(msg_level) << ']' << '\t'
		<< message;
	log_full_message(ss.str());
};

}
