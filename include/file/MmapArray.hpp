
#pragma once

#include <cstdint>

#include "MmapFile.hpp"

namespace file {

// Memory map array. Opens file and treats its content as array of `Data`
template <typename Data>
class MmapArray {
public:
	using iterator = Data*;
	using const_iterator = Data const*;

	MmapArray(const std::filesystem::path& file);
	MmapArray() = delete;
	~MmapArray() = default;

	// Get array size
	std::size_t size() const;

	// Iterator to first element
	iterator begin();
	// Iterator to element past last
	iterator end();
	// Iterator to first element
	const_iterator begin() const;
	// Iterator to element past last
	const_iterator end() const;

private:
	MmapFile file_;		// Memory mapped file
	Data* data;			// Pointer to data in memory
	std::size_t length;	// Array size

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
