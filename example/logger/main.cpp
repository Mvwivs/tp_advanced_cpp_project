
#include <iostream>
#include <memory>
#include <cassert>

#include "logger/Logger.hpp"

int main() {

	auto log = std::unique_ptr<logger::BaseLogger>(new logger::StdoutLogger());

	log->debug("Some debug info");
	log->info("Breaking news: ...");
	log->warn("Conversion from long to int");
	log->error("Main engine failure");

	log->set_level(logger::Level::warn);
	log->info("This message should not be shown");
	log->flush();

	logger::Logger::get_instance().set_global_logger(logger::create_file_logger("log.log"));

	logger::debug("Some debug info");
	logger::info("Breaking news: ...");
	logger::warn("Conversion from long to int");
	logger::error("Main engine failure");

	logger::Logger::get_instance().get_global_logger().set_level(logger::Level::warn);
	logger::info("This message should not be shown");
	logger::Logger::get_instance().get_global_logger().flush();

	return 0;
}
