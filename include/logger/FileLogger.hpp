
#pragma once

#include <fstream>
#include <filesystem>

#include "BaseLogger.hpp"

namespace logger {

class FileLogger : public BaseLogger {
public:
	FileLogger(const std::filesystem::path& filename);
	virtual ~FileLogger() = default;

	virtual void flush() override;

protected:
	virtual void log_full_message(const std::string& full_message) override;

private:
	std::ofstream log_file;
};
}
