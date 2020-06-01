
#include "file/DbServer.hpp"

#include <sstream>
#include <algorithm>

namespace file {

using namespace std::string_literals;

constexpr std::size_t step = 1024;

DbServer::DbServer(const tcp::Address& address, const std::filesystem::path& file) :
	http::Server(address),
	db(file),
	index(db, step) {
}

http::HTTP::Response DbServer::onRequest(const http::HTTP::Request& request) {
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

std::optional<Data> DbServer::getData(std::uint64_t key) const {
	const auto& [start, end] = index.getInterval(key);
	if (start == Index::npos) {
		return {};
	}
	auto it = std::lower_bound(db.begin() + start, db.begin() + end, key, 
		[] (const Record& a, std::uint64_t b) {
			return a.key < b;
		});
	if (it == db.end() || it->key != key) {
		return {};
	}
	return it->data;
}

http::HTTP::Response DbServer::error404() const {
	http::HTTP::Response resp {
		{http::HTTP::Version::HTTP_1_1, http::HTTP::StatusCode{404}},
		{{"Connection", "close"}},
		{}
	};
	return resp;
}

}
