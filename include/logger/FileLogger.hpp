
#include <fstream>
#include <filesystem>

#include "BaseLogger.hpp"

namespace logger {

class FileLogger : public BaseLogger {
public:
	FileLogger(const std::filesystem::path& filename):
		log_file(filename) {
	}
	virtual ~FileLogger() = default;

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
