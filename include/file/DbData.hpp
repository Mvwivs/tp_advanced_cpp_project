
#pragma once

#include <cstdint>

namespace file {

using Key = std::uint64_t;

struct Data {
	std::uint8_t payload[20];

};

struct Record {
	Key key;
	Data data;
};

}
