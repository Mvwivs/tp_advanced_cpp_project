
#pragma once

#include <cstdint>
#include <optional>

#include "http/Server.hpp"
#include "DbData.hpp"
#include "MmapArray.hpp"
#include "Index.hpp"

namespace file {

class DbServer : public http::Server {
public:
	DbServer(const tcp::Address& address, const std::filesystem::path& file);
	~DbServer() = default;

	http::HTTP::Response onRequest(const http::HTTP::Request& request) override;

private:
	std::optional<Data> getData(std::uint64_t key) const;

	http::HTTP::Response error404() const;

private:
	MmapArray<Record> db;
	Index index;

	static const std::size_t step = 1024;
};

}
