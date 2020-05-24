
#include "file/Index.hpp"

namespace file {

Index::Index(const MmapArray<Record>& db, std::size_t step):
	step_(step) {
	for (std::size_t i = 0; i < db.size(); i += step) {
		const auto& [key, data] = *(db.begin() + i);
		index.emplace(key, i);
	}
	std::uint64_t lastKey = std::prev(db.end())->key;
	std::size_t lastOffset = db.size();
	if (lastKey % step != 0) {
		index.emplace(lastKey, lastOffset);
	}
}

std::pair<std::size_t, std::size_t> Index::getInterval(std::uint64_t key) const {
	if (key < index.begin()->first || key > std::prev(index.end())->first) { // out of bounds
		return { npos, npos };
	}
	auto blockStart =  index.upper_bound(key);
	if (blockStart == index.end()) {
		return { blockStart->second, blockStart->second }; // hit last element
	}
	return { std::prev(blockStart)->second, blockStart->second };
}

}
