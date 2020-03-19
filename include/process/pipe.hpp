
#pragma once

#include <stdexcept>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>

namespace process {

using namespace std::string_literals;

class Descriptor {
public:
	virtual size_t write(const void* data, size_t len) = 0;
	virtual size_t read(void* data, size_t len) = 0;
	virtual void close() = 0;
};

class DuplexDescriptor : public Descriptor {
public:
	explicit DuplexDescriptor();

	DuplexDescriptor(const DuplexDescriptor& other) = delete;
	DuplexDescriptor& operator=(const DuplexDescriptor& other) = delete;

	DuplexDescriptor(DuplexDescriptor&& other);

	DuplexDescriptor& operator=(DuplexDescriptor&& other);

	~DuplexDescriptor();

	explicit DuplexDescriptor(int input, int output);

	void closeIn();

	void closeOut();

	bool isInAvailable() const;

	size_t write(const void* data, size_t len) override;

	size_t read(void* data, size_t len) override;

	void toCinCout();

	void close() override;

private:
	static void closeDescriptor(int& fd);

private:
	int in;
	int out;

};

using Pipe = std::pair<DuplexDescriptor, DuplexDescriptor>;

Pipe createPipe(); // TODO: replace with factory

} // namespace process
