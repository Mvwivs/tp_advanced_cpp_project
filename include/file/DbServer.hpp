
#pragma once

#include <cstdint>
#include <optional>

#include "http/Server.hpp"
#include "DbData.hpp"
#include "MmapArray.hpp"
#include "Index.hpp"

namespace file {

// Server provides access to data in file 
class DbServer : public http::Server {
public:
	DbServer(const tcp::Address& address, const std::filesystem::path& file);
	~DbServer() = default;

	// Form response on client request
	http::HTTP::Response onRequest(const http::HTTP::Request& request) override;

private:
	// Get data from file
	std::optional<Data> getData(std::uint64_t key) const;
	// Response with 404 error
	http::HTTP::Response error404() const;

private:
	MmapArray<Record> db;	// File access array
	Index index;			// Index to increase access speed

};

}
