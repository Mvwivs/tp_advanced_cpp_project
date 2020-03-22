
#include <iostream>
#include <memory>
#include <cassert>

#include "logger/StdoutLogger.hpp"

int main() {

	auto log = std::unique_ptr<logger::BaseLogger>(new logger::StdoutLogger());

	log->debug("Some debug info");
	log->info("Breaking news: ...");
	log->warn("Conversion from long to int");
	log->error("Main engine failure");

	log->set_level(logger::Level::warn);
	log->info("This message should not be shown");
	log->flush();

	return 0;
}
