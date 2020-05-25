
#pragma once

#include <map>
#include <cstdint>

#include "MmapArray.hpp"
#include "DbData.hpp"

namespace file {

// Creates index for <Key, Data> array
class Index {
public:
	Index(const MmapArray<Record>& db, std::size_t step);
	// Index(const std::filesystem::path& file); // TODO: add loading from file
	Index() = delete;
	~Index() = default;

	// Get offset interval in wich key is located
	std::pair<std::size_t, std::size_t> getInterval(std::uint64_t key) const;

	static const std::size_t npos;	// Offset out of file

private:
	std::map<Key, std::size_t> index;	// Map holding index

};

}
