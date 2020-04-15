
#pragma once

#include <iostream>

#include "BaseLogger.hpp"

namespace logger {

// Logger for writing to stderr
class StderrLogger : public BaseLogger {
public:
	StderrLogger() = default;
	virtual ~StderrLogger() = default;

	// flush logger buffers
	void flush() override;

protected:
	// write message to log target
	void log_full_message(const std::string& full_message) override;
};

}
