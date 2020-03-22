
#include <memory>

#include "BaseLogger.hpp"
#include "StdoutLogger.hpp"

namespace logger {

class Logger {
public:
	Logger(const Logger& othrer) = delete;
	Logger& operator=(const Logger& other) = delete;
	static Logger& get_instance() {
		static Logger instance;
		return instance;
	}

	std::shared_ptr<BaseLogger> get_global_logger() {
		return global_logger_;
	}

	void set_global_logger(std::shared_ptr<BaseLogger> new_logger) {
		global_logger_ = std::move(new_logger);
	}

private:
	Logger() = default;
	std::shared_ptr<BaseLogger> global_logger_;
};

void create_stdout_logger() {
	Logger& log = Logger::get_instance();
	log.set_global_logger(std::make_unique<StdoutLogger>());
}

void debug(const std::string& message) {
	auto log = Logger::get_instance().get_global_logger();
	log->debug(message);
}

} // namespace logger

