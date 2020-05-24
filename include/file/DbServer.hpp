
#pragma once

#include <cstdint>
#include <sstream>
#include <algorithm>
#include <optional>

#include "http/Server.hpp"
#include "DbData.hpp"
#include "MmapArray.hpp"
#include "Index.hpp"

namespace file {

using namespace std::string_literals;

class DbServer : public http::Server {
public:
	DbServer(const tcp::Address& address, const std::filesystem::path& file) :
		http::Server(address),
		db(file),
		index(db, step) {
	}
	~DbServer() = default;

	http::HTTP::Response onRequest(const http::HTTP::Request& request) override {
		if (!request.parameters || request.parameters->size() != 1) {
			return error404();
		}
		const auto& [name, value] = (*request.parameters)[0];
		if (name != "key") {
			return error404();
		}
		std::uint64_t key;
		try {
			std::istringstream iss(value);
			iss >> key;
		}
		catch (const std::istream::failure& e) {
			return error404();
		}

		auto data = getData(key);
		if (!data) {
			return error404();
		}
		std::string d{reinterpret_cast<char*>(&data->payload), sizeof(data->payload)};
		http::HTTP::Response resp {
			{http::HTTP::Version::HTTP_1_1, http::HTTP::StatusCode{200}},
			{{"Connection", "Keep-Alive"},
			{"Content-Type", "application/json"}},
			{"{\"data\": \""s + d + "\"}"s}
		};
		return resp;
	}

private:
	std::optional<Data> getData(std::uint64_t key) const {
		const auto& [start, end] = index.getInterval(key);
		if (start == std::size_t(-1)) {
			return {};
		}
		auto it = std::lower_bound(db.begin() + start, db.begin() + end, key, 
			[] (const std::pair<std::uint64_t, Data>& a, std::uint64_t b) {
				return a.first < b;
			});
		if (it == db.end() || it->first != key) {
			return {};
		}
		return it->second;
	}

	http::HTTP::Response error404() const {
		http::HTTP::Response resp {
			{http::HTTP::Version::HTTP_1_1, http::HTTP::StatusCode{404}},
			{{"Connection", "close"}},
			{}
		};
		return resp;
	}

private:
	MmapArray<std::pair<std::uint64_t, Data>> db;
	Index index;

	static const std::size_t step = 1024;
};

}
