
#include <fstream>

#include "BaseLogger.hpp"

namespace logger {

class StderrLogger : public BaseLogger {
public:
	StderrLogger(const std::string& filename):
		log_file(filename) {
	}
	virtual ~StderrLogger() = default;

	virtual void flush() override {
		log_file.flush();
	}

protected:
	virtual void log_full_message(const std::string& full_message) override {
		log_file << full_message << std::endl;
	}

private:
	std::ofstream log_file;
};
}
