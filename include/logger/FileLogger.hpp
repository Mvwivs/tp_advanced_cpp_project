
#pragma once

#include <fstream>
#include <filesystem>

#include "BaseLogger.hpp"

namespace logger {

// Logger for writing to file
class FileLogger : public BaseLogger {
public:
	FileLogger(const std::filesystem::path& filename);
	virtual ~FileLogger() = default;

	// flush logger buffers
	virtual void flush() override;

protected:
	// write message to log target
	virtual void log_full_message(const std::string& full_message) override;

private:
	std::ofstream log_file; // logger output file
};
}
