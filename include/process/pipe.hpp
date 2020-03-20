
#pragma once

#include <stdexcept>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>

namespace process {

// Combines two unidirectional descriptors into one bidirectional
class DuplexDescriptor {
public:
	DuplexDescriptor();
	DuplexDescriptor(const DuplexDescriptor& other) = delete;
	DuplexDescriptor& operator=(const DuplexDescriptor& other) = delete;
	DuplexDescriptor(DuplexDescriptor&& other);
	DuplexDescriptor& operator=(DuplexDescriptor&& other);
	~DuplexDescriptor();

	DuplexDescriptor(int input, int output);
	// Close input descriptor
	void closeIn();
	// Close output descriptor
	void closeOut();
	// Check if input descriptor is available for reading
	bool isInAvailable() const;
	// Write data to output descriptor
	size_t write(const void* data, size_t len);
	// Read data from input desctiprot
	size_t read(void* data, size_t len);
	// Redirect descriptors to stdin/stdout
	void redirectToStd();
	// Close descriptors
	void close();

private:
	int in_;		// Input (read) descriptor
	int out_;	// Output (write) descriptor

};

// try close underlying descriptor and set it to -1 
void closeDescriptor(int& fd);

// biderectional pipe, consisting of two connected biderectional descriptors
using DuplexPipe = std::pair<DuplexDescriptor, DuplexDescriptor>;

// Create two connected bidirectional read/write descriptors
DuplexPipe createDuplexPipe(); // TODO: replace with factory

} // namespace process
