
#pragma once

#include <iostream>

#include "BaseLogger.hpp"

namespace logger {

class StderrLogger : public BaseLogger {
public:
	StderrLogger() = default;
	virtual ~StderrLogger() = default;

	virtual void flush() override;

protected:
	virtual void log_full_message(const std::string& full_message) override;
};

}
