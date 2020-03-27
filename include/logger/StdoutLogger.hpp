
#pragma once

#include <iostream>

#include "BaseLogger.hpp"

namespace logger {

class StdoutLogger : public BaseLogger {
public:
	StdoutLogger() = default;
	virtual ~StdoutLogger() = default;

	virtual void flush() override;

protected:
	virtual void log_full_message(const std::string& full_message) override;
};

}
