
#pragma once

#include <cstdint>

namespace file {

// Key for db
using Key = std::uint64_t;

// Example data for db
struct Data {
	std::uint8_t payload[20];

};

// One record in db
struct Record {
	Key key;
	Data data;
};

}
