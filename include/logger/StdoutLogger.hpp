
#include <iostream>

#include "BaseLogger.hpp"

namespace logger {

class StdoutLogger : public BaseLogger {
public:
	StdoutLogger() = default;
	virtual ~StdoutLogger() = default;

	virtual void flush() override {
		std::cout.flush();
	}

protected:
	virtual void log_full_message(const std::string& full_message) override {
		std::cout << full_message << std::endl;
	}
};

}

