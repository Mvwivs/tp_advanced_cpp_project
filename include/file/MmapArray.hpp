
#pragma once

#include "MmapFile.hpp"

namespace file {

template <typename Data>
class MmapArray {
public:
	using iterator = Data*;
	using const_iterator = Data const*;

	MmapArray(const std::filesystem::path& file);
	MmapArray() = delete;
	~MmapArray() = default;

	std::size_t size() const;

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

private:
	MmapFile file_;
	Data* data;
	std::size_t length;

};

template <typename Data>
MmapArray<Data>::MmapArray(const std::filesystem::path& file):
	file_(file) {

	data = reinterpret_cast<Data*>(file_.mmap());
	length = file_.size() / sizeof(Data);
}

template <typename Data>
std::size_t MmapArray<Data>::size() const {
	return length;
}

template <typename Data>
typename MmapArray<Data>::iterator MmapArray<Data>::begin() {
	return data;
}
template <typename Data>
typename MmapArray<Data>::iterator MmapArray<Data>::end() {
	return data + length;
}
template <typename Data>
typename MmapArray<Data>::const_iterator MmapArray<Data>::begin() const {
	return data;
}
template <typename Data>
typename MmapArray<Data>::const_iterator MmapArray<Data>::end() const {
	return data + length;
}


}
