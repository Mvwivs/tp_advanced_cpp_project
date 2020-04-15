
#pragma once

#include <iostream>

#include "BaseLogger.hpp"

namespace logger {

// Logger for writing to stdout
class StdoutLogger : public BaseLogger {
public:
	StdoutLogger() = default;
	virtual ~StdoutLogger() = default;

	// flush logger buffers
	void flush() override;

protected:
	// write message to log target
	void log_full_message(const std::string& full_message) override;
};

}
