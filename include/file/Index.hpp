
#pragma once

#include <map>

#include "MmapArray.hpp"
#include "DbData.hpp"

namespace file {

class Index {
public:

	Index(const MmapArray	<std::pair<std::uint64_t, Data>>& db, std::size_t step):
		step_(step) {
		for (std::size_t i = 0; i < db.size(); i += step) {
			const auto& [key, data] = *(db.begin() + i);
			index.emplace(key, i);
		}
		std::uint64_t lastKey = std::prev(db.end())->first;
		std::size_t lastOffset = db.size();
		if (lastKey % step != 0) {
			index.emplace(lastKey, lastOffset);
		}
	}
	// Index(const std::filesystem::path& file); // TODO: add loading from file
	Index() = delete;
	~Index() = default;

	std::pair<std::size_t, std::size_t> getInterval(std::uint64_t key) const {
		if (key < index.begin()->first || key > std::prev(index.end())->first) { // TODO: save
			return { -1, -1 };
		}
		auto blockStart =  index.upper_bound(key);
		if (blockStart == index.end()) {
			return { blockStart->second, blockStart->second }; // hit last element
		}
		return { std::prev(blockStart)->second, blockStart->second };
	}

private:
	std::map<std::uint64_t, std::size_t> index;
	std::size_t step_;
};

}
