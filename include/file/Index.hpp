
#pragma once

#include <map>

#include "MmapArray.hpp"
#include "DbData.hpp"

namespace file {

class Index {
public:
	Index(const MmapArray<Record>& db, std::size_t step);
	// Index(const std::filesystem::path& file); // TODO: add loading from file
	Index() = delete;
	~Index() = default;

	std::pair<std::size_t, std::size_t> getInterval(std::uint64_t key) const;

private:
	std::map<Key, std::size_t> index;
	std::size_t step_;
};

}
