
#pragma once

#include "MmapFile.hpp"

namespace file {

template <typename Data>
class MmapArray {
public:
	using iterator = Data*;
	using const_iterator = Data const*;

	MmapArray(const std::filesystem::path& file):
		file_(file) {

		data = reinterpret_cast<Data*>(file_.mmap_);
		length = file_.size / sizeof(Data);
	}
	MmapArray() = delete;
	~MmapArray() = default;

	iterator begin() {
		return data;
	}
	iterator end() {
		return data + length;
	}
	const_iterator begin() const {
		return data;
	}
	const_iterator end() const {
		return data + length;
	}

private:
	MmapFile file_;
	Data* data;
	std::size_t length;

};

}
