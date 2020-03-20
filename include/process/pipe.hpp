
#pragma once

#include <stdexcept>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>

namespace process {

// Interface for descriptor access
class Descriptor {
public:
	// Write data to descriptor
	virtual size_t write(const void* data, size_t len) = 0;
	// Read data from descriptor
	virtual size_t read(void* data, size_t len) = 0;
	// Close descriptor
	virtual void close() = 0;
};

// Combines two unidirectional descriptors into one bidirectional
class DuplexDescriptor : public Descriptor {
public:
	explicit DuplexDescriptor();
	DuplexDescriptor(const DuplexDescriptor& other) = delete;
	DuplexDescriptor& operator=(const DuplexDescriptor& other) = delete;
	DuplexDescriptor(DuplexDescriptor&& other);
	DuplexDescriptor& operator=(DuplexDescriptor&& other);
	~DuplexDescriptor();

	explicit DuplexDescriptor(int input, int output);
	// Close input descriptor
	void closeIn();
	// Close output descriptor
	void closeOut();
	// Check if input descriptor is available for reading
	bool isInAvailable() const;
	// Write data to output descriptor
	size_t write(const void* data, size_t len) override;
	// Read data from input desctiprot
	size_t read(void* data, size_t len) override;
	// Redirect descriptors to stdin/stdout
	void redirectToStd();
	// Close descriptors
	void close() override;

private:
	// Close underlying descriptor
	static void closeDescriptor(int& fd);

private:
	int in;		// Input (read) descriptor
	int out;	// Output (write) descriptor

};

// Pipe, consisting of two connected biderectional descriptors
using Pipe = std::pair<DuplexDescriptor, DuplexDescriptor>;

// Create two connected bidirectional read/write descriptors
Pipe createPipe(); // TODO: replace with factory

} // namespace process
